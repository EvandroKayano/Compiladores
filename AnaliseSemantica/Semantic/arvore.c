#include <stdio.h>
#include "arvore.h"

Node *criar_no(Tipo_No tipo, Node *p1, Node *p2, Node *p3, Node *p4){
    Node *no = (Node *)malloc(sizeof(Node));

    no->tipo = tipo;
    no->p1 = p1;
    no->p2 = p2;    
    no->p3 = p3;
    no->p4 = p4;

    if (p1 != NULL) no->linha = p1->linha;
    else if (p2 != NULL) no->linha = p2->linha;
    else if (p3 != NULL) no->linha = p3->linha;
    else if (p4 != NULL) no->linha = p4->linha;
    else no->linha = -1;

    return no;
}

Node *criar_folha_num(int valor){ // Cria uma folha do tipo num
    Node *no = (Node *)malloc(sizeof(Node));
    no->tipo = NODE_NUM;
    no->valor.num_val = valor;

    no->p1 = no->p2 = no->p3 = no->p4 = NULL;
    return no;
}

Node *criar_folha_id(char *valor, int linha){ // Cria uma folha do tipo id
    Node *no = (Node *)malloc(sizeof(Node));
    no->tipo = NODE_ID;
    no->valor.id_val = valor;
    no->linha = linha;

    no->p1 = no->p2 = no->p3 = no->p4 = NULL;
    return no;
}

static const char* get_node_label(Node *node) {
    static char label[100]; 
    
    // Nome que aparece na arvore
    const char *tipo_str;
    switch(node->tipo) {
        case NODE_PROGRAMA: tipo_str = "PROGRAMA"; break;
        case NODE_DECL_LISTA: tipo_str = "DECL_LISTA"; break;
        case NODE_VAR_DECL: tipo_str = "VAR_DECL"; break;
        case NODE_FUN_DECL: tipo_str = "FUN_DECL"; break;
        case NODE_COMPOSTO_DECL: tipo_str = "COMPOSTO_DECL"; break;
        case NODE_IF: tipo_str = "IF"; break;
        case NODE_WHILE: tipo_str = "WHILE"; break;
        case NODE_RETURN: tipo_str = "RETURN"; break;
        case NODE_ASSIGN: tipo_str = "ASSIGN"; break;
        case NODE_RELACIONAL: tipo_str = "RELACIONAL"; break;
        case NODE_SOMA: tipo_str = "SOMA"; break;
        case NODE_MULT: tipo_str = "MULT"; break;
        case NODE_VAR: tipo_str = "VAR"; break;
        case NODE_CALLBACK: tipo_str = "CALLBACK"; break;
        case NODE_ID: 
            if(strcmp(node->valor.id_val,"int") == 0 || strcmp(node->valor.id_val,"void") == 0)
                tipo_str = "TIPO";
            else
                tipo_str = "ID"; 
            break;
        case NODE_NUM: tipo_str = "NUM"; break;
        default: tipo_str = "DESCONHECIDO";
    }

    // Adiciona o valor se ele existir
    switch(node->tipo) {
        case NODE_ID:
        case NODE_RELACIONAL:
        case NODE_SOMA:
        case NODE_MULT:
            snprintf(label, sizeof(label), "%s (%s)", tipo_str, node->valor.id_val);
            break;
        case NODE_NUM:
            snprintf(label, sizeof(label), "%s (%d)", tipo_str, node->valor.num_val);
            break;
        default:
            snprintf(label, sizeof(label), "%s", tipo_str);
    }
    return label;
}

static void print_arvore_ascii_recursive(Node *node, char* prefix, int is_last) {
    if (node == NULL) {
        return;
    }

    // Enfeite
    printf("%s", prefix);
    if (is_last) {
        printf("└─ "); // "L" para o último filho
    } else {
        printf("├─ "); // "T" para os outros
    }

    // Nome do nó
    printf("[%s]", get_node_label(node));

    // Se for ID e não for tipo, printa a linha
    if(node->tipo == NODE_ID && strcmp(node->valor.id_val,"int") && strcmp(node->valor.id_val, "void"))
        printf(" (linha: %d)", node->linha);

    printf("\n");

    /* 
       ajusta a formatação do prefixo para os filhos
       além de organizar a quantidade de espaços para a estrutura da árvore
       se for o último filho, adiciona espaços, senão adiciona uma linha vertical
       por fim, a quantidade de espaços é passada recursivamente para os filhos
    */
    char new_prefix[512];
    snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix, is_last ? "   " : "│  ");

    // Pega o último filho não nulo, para colocar o └─ 
    Node *children[4] = {node->p1, node->p2, node->p3, node->p4};
    Node *last_child = NULL;
    for (int i = 3; i >= 0; i--) { // Itera de p4 para p1
        if (children[i] != NULL) {
            last_child = children[i];
            break;
        }
    }

    for (int i = 0; i < 4; i++) {
        if (children[i] != NULL) {
            // eu mando uma flag dizendo se é o último filho
            print_arvore_ascii_recursive(children[i], new_prefix, children[i] == last_child);
        }
    }

}

void print_arvore(Node *raiz, int level){
    if (raiz == NULL) {
        return;
    }
    
    // Imprime a raiz
    printf("[%s]\n", get_node_label(raiz));

    // Encontra o último filho da raiz
    Node *children[4] = {raiz->p1, raiz->p2, raiz->p3, raiz->p4};
    Node *last_child = NULL;
    for (int i = 3; i >= 0; i--) { // Itera de p4 para p1
        if (children[i] != NULL) {
            last_child = children[i];
            break;
        }
    }

    // Chama recursivamente para os filhos da raiz e manda qual é o último filho, para por o prefixo correto
    for (int i = 0; i < 4; i++) {
        if (children[i] != NULL) {
            // O prefixo inicial é ""
            print_arvore_ascii_recursive(children[i], "", children[i] == last_child);
        }
    }
}

void free_tree(Node *raiz){
    if (raiz == NULL) {
        return;
    }

    /* Libera os filhos primeiro (pós-ordem) */
    free_tree(raiz->p1);
    free_tree(raiz->p2);
    free_tree(raiz->p3);
    free_tree(raiz->p4);

    switch(raiz->tipo) {
        case NODE_ID:
        case NODE_SOMA:
        case NODE_MULT:
        case NODE_RELACIONAL:
            if (raiz->valor.id_val != NULL) {
                free(raiz->valor.id_val);
            }
            break;
        default:
            break;
    }

    free(raiz);
}

