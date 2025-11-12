#include <stdio.h>
#include "arvore.h"

Node *criar_no(Tipo_No tipo, Node *p1, Node *p2, Node *p3, Node *p4){
    Node *no = (Node *)malloc(sizeof(Node));

    no->tipo = tipo;
    no->p1 = p1;
    no->p2 = p2;    
    no->p3 = p3;
    no->p4 = p4;
    return no;
}

Node *criar_folha_num(int valor){ // Cria uma folha para num
    Node *no = (Node *)malloc(sizeof(Node));
    no->tipo = NODE_NUM;
    no->valor.num_val = valor;

    no->p1 = no->p2 = no->p3 = no->p4 = NULL;
    return no;
}

Node *criar_folha_id(char *valor){ // Cria uma folha para id
    Node *no = (Node *)malloc(sizeof(Node));
    no->tipo = NODE_ID;
    no->valor.id_val = valor;

    no->p1 = no->p2 = no->p3 = no->p4 = NULL;
    return no;
}

static const char* get_node_label(Node *node) {
    // Buffer estático para construir o label
    static char label[100]; 
    
    // Pega o nome do tipo do nó (ex: "VAR_DECL")
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
        case NODE_ID: tipo_str = "ID"; break;
        case NODE_NUM: tipo_str = "NUM"; break;
        default: tipo_str = "DESCONHECIDO";
    }

    // Adiciona o valor se ele existir
    switch(node->tipo) {
        case NODE_ID:
        case NODE_RELACIONAL:
        case NODE_SOMA:
        case NODE_MULT:
            // Formata como "TIPO (valor)"
            snprintf(label, sizeof(label), "%s (%s)", tipo_str, node->valor.id_val);
            break;
        case NODE_NUM:
            // Formata como "TIPO (valor)"
            snprintf(label, sizeof(label), "%s (%d)", tipo_str, node->valor.num_val);
            break;
        default:
            // Apenas usa o tipo
            snprintf(label, sizeof(label), "%s", tipo_str);
    }
    return label;
}

static void print_arvore_ascii_recursive(Node *node, char* prefix, int is_last) {
    if (node == NULL) {
        return;
    }

    // 1. Imprime o prefixo e o conector
    printf("%s", prefix);
    if (is_last) {
        printf("└─ "); // "L" para o último filho
    } else {
        printf("├─ "); // "T" para os outros
    }

    // 2. Imprime o label do nó
    printf("[%s]\n", get_node_label(node));

    // 3. Prepara o novo prefixo para os filhos
    char new_prefix[512];
    snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix, is_last ? "   " : "│  ");

    // 4. Encontra o último filho não nulo
    Node *children[4] = {node->p1, node->p2, node->p3, node->p4};
    Node *last_child = NULL;
    for (int i = 3; i >= 0; i--) { // Itera de p4 para p1
        if (children[i] != NULL) {
            last_child = children[i];
            break;
        }
    }

    // 5. Chama recursivamente para os filhos
    for (int i = 0; i < 4; i++) {
        if (children[i] != NULL) {
            print_arvore_ascii_recursive(children[i], new_prefix, children[i] == last_child);
        }
    }
}

void print_arvore(Node *raiz, int level){
if (raiz == NULL) {
        return;
    }
    
    // 1. Imprime a raiz
    printf("[%s]\n", get_node_label(raiz));

    // 2. Encontra o último filho da raiz
    Node *children[4] = {raiz->p1, raiz->p2, raiz->p3, raiz->p4};
    Node *last_child = NULL;
    for (int i = 3; i >= 0; i--) { // Itera de p4 para p1
        if (children[i] != NULL) {
            last_child = children[i];
            break;
        }
    }

    // 3. Chama recursivamente para os filhos da raiz
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

    /* Libera a string 'id_val' se ela foi alocada com strdup() */
    switch(raiz->tipo) {
        case NODE_ID:
        case NODE_SOMA:
        case NODE_MULT:
        case NODE_RELACIONAL:
            if (raiz->valor.id_val != NULL) {
                free(raiz->valor.id_val);
            }
            break;
        /* Adicione outros 'case' se você usar strdup para eles */
        default:
            break;
    }

    /* Finalmente, libere o próprio nó */
    free(raiz);
}