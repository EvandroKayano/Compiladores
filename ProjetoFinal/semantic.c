#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "semantic.h"

Pilha *topo_pilha = NULL;

// lista de erros   
MensagemErro *errors_init = NULL;
MensagemErro *errors_end = NULL;
int n_erros = 0;


int hash(char *nome){
    unsigned long aux = 0;
    int i = 0;
    while (nome[i] != '\0') {
        aux = ((aux << 4) + nome[i]) % HASH_SIZE;
        ++i;
    }
    return (int)aux;
}


#pragma region tabela de simbolos

TabelaSimbolos *tabela_inicio = NULL;
TabelaSimbolos *tabela_fim = NULL;
int count = 1;

//  poe o simbolo na tabela de símbolos
void add_tabela(char *nome, char *escopo, TipoDado tipo, Categoria cat, int linha, int hash_index) {
    TabelaSimbolos *novo = (TabelaSimbolos *)malloc(sizeof(TabelaSimbolos));
    
    novo->id_entrada = count++;
    novo->nome = strdup(nome);
    novo->escopo = strdup(escopo);
    novo->linha = linha;
    novo->prox = NULL;
    novo->hash_index = hash_index;

    // enum -> string
    if (cat == VAR) novo->cat = strdup("var");
    else if (cat == VET) novo->cat = strdup("vet");
    else novo->cat = strdup("fun");

    if (tipo == TIPO_INT) novo->tipo = strdup("int");
    else if (tipo == TIPO_VOID) novo->tipo = strdup("void");
    else novo->tipo = strdup("erro");

    // insere na lista
    if (tabela_inicio == NULL) {
        tabela_inicio = novo;
        tabela_fim = novo;
    } else {
        tabela_fim->prox = novo;
        tabela_fim = novo;
    }
}


void print_tabela() {
    printf("\n\n");
    printf("==========================================================================================\n");
    printf("                                    TABELA DE SIMBOLOS                                    \n");
    printf("==========================================================================================\n");
    
    // formatação
    printf("| %-7s | %-8s | %-15s | %-15s | %-8s | %-9s | %-6s |\n", 
           "ENTRADA", "HASH KEY", "NOME ID", "ESCOPO", "TIPO ID", "TIPO DADO", "LINHA");
    printf("|---------|----------|-----------------|-----------------|----------|-----------|--------|\n");


    TabelaSimbolos *atual = tabela_inicio;
    while (atual != NULL) {
        printf("| %-7d | %-8d | %-15s | %-15s | %-8s | %-9s | %-6d |\n", 
               atual->id_entrada, 
               atual->hash_index,
               atual->nome, 
               atual->escopo, 
               atual->cat, 
               atual->tipo, 
               atual->linha);
        
        atual = atual->prox;
    }
    printf("==========================================================================================\n");
}


void free_tabela() {
    TabelaSimbolos *atual = tabela_inicio;
    while (atual != NULL) {
        TabelaSimbolos *temp = atual;
        atual = atual->prox;
        
        free(temp->nome);
        free(temp->escopo);
        free(temp->cat);
        free(temp->tipo);
        free(temp);
    }
    tabela_inicio = NULL;
    tabela_fim = NULL;
}


#pragma endregion


#pragma region erros

void registrar_erro(int linha, const char *msg, ...){
    MensagemErro *novo = (MensagemErro *)malloc(sizeof(MensagemErro));
    novo->linha = linha;
    novo->prox = NULL;

    va_list args;
    va_start(args, msg);
    vsnprintf(novo->mensagem, sizeof(novo->mensagem), msg, args);
    va_end(args);

    if (errors_init == NULL) {
        errors_init = novo;
        errors_end = novo;
    } 
    else {
        errors_end->prox = novo;
        errors_end = novo;
    }
    n_erros++;
}


void imprimir_erros(){
    MensagemErro *atual = errors_init;
    while(atual != NULL){
        printf( "Erro Semantico (Linha %d): %s\n", atual->linha, atual->mensagem);
        MensagemErro *aux = atual;
        atual = atual->prox;
        free(aux);
    }
    errors_init = NULL;
    errors_end = NULL;
}

#pragma endregion


#pragma region escopos

void push_escopo(char *nome){ 
    Pilha *novo = (Pilha *) malloc (sizeof(Pilha));
    for(int i = 0; i < HASH_SIZE; i++){
        novo->hashes[i] = NULL;
    }
    novo->pai = topo_pilha;
    novo->nivel = topo_pilha == NULL ? 0 : topo_pilha->nivel + 1;
    novo->nome_escopo = strdup(nome);
    topo_pilha = novo;
}


void pop_escopo(){
    if(topo_pilha != NULL){

        // liberar os símbolos
        for (int i = 0; i < HASH_SIZE; i++) {
            Simbolo *s = topo_pilha->hashes[i];
            while (s != NULL) {
                Simbolo *temp = s;
                s = s->prox;
                
                free(temp->nome);
                free(temp);
            }
        }

        // liberar a pilha
        Pilha *aux = topo_pilha;
        topo_pilha = topo_pilha->pai;
        free(aux->nome_escopo);
        free(aux);
    }
}

#pragma endregion


#pragma region símbolos

Simbolo *buscar_simbolo(char *nome){
    Pilha *pilha_atual = topo_pilha;
    int index = hash(nome);

    while(pilha_atual != NULL){
        Simbolo *s = pilha_atual->hashes[index];
        while(s != NULL){
            if(strcmp(s->nome, nome) == 0) return s;
            s = s->prox;
        }
        pilha_atual = pilha_atual->pai;
    }
    return NULL;
}

void add_simbolo(char *nome, TipoDado tipo, Categoria cat, int linha, int num_params){
    if(topo_pilha == NULL) return;

    int index = hash(nome);
    Simbolo *s = topo_pilha->hashes[index];

    while(s != NULL){
        if(strcmp(s->nome, nome) == 0){
            registrar_erro(linha, "Declaracao duplicada do simbolo '%s'.", nome);   
            return;
        }
        s = s->prox;
    }

    Simbolo *novo = (Simbolo *) malloc (sizeof(Simbolo));
    novo->nome = strdup(nome);
    novo->tipo = tipo;
    novo->cat = cat;
    novo->linha = linha;
    novo->num_params = num_params;

    novo->prox = topo_pilha->hashes[index];
    topo_pilha->hashes[index] = novo;

    // ao inserir o símbolo, também adiciona na tabela geral
    add_tabela(nome, topo_pilha->nome_escopo, tipo, cat, linha, index);
}

#pragma endregion


#pragma region análise da árvore

TipoDado ler_tipo(Node *node) {
    if (node == NULL) return TIPO_ERRO; 

    // tipos primitivos (usados em declarações)
    if (node->tipo == NODE_ID) {
        if (strcmp(node->valor.id_val, "void") == 0) return TIPO_VOID;
        if (strcmp(node->valor.id_val, "int") == 0) return TIPO_INT;
        // Se for um ID solto que não é int/void e não caiu nos casos abaixo, é estranho
        return TIPO_ERRO; 
    }

    // números literais
    if (node->tipo == NODE_NUM) return TIPO_INT;

    // operações matemáticas (Sempre resultam em INT)
    // Mesmo que os operandos estejam errados, o resultado da soma "seria" int
    if (node->tipo == NODE_SOMA || node->tipo == NODE_MULT) return TIPO_INT;
    if (node->tipo == NODE_RELACIONAL) return TIPO_INT; // Em C, true/false é 1/0 (int)

    // variáveis: Buscamos na tabela
    if (node->tipo == NODE_VAR) {
        if (node->p1 == NULL) return TIPO_ERRO;
        Simbolo *s = buscar_simbolo(node->p1->valor.id_val);
        // Se encontrou, retorna o tipo real. Se não, retorna ERRO.
        return s ? s->tipo : TIPO_ERRO; 
    }

    // chamadas de Função
    if (node->tipo == NODE_CALLBACK) {
        if (node->p1 == NULL) return TIPO_ERRO;
        Simbolo *s = buscar_simbolo(node->p1->valor.id_val);
        return s ? s->tipo : TIPO_ERRO;
    }

    // atribuição por função
    if (node->tipo == NODE_ASSIGN) {
        return ler_tipo(node->p1);
    }

    return TIPO_ERRO;
}

// adiciona o numero de parametros para uma função
int contar_parametros(Node *lista_parametros){
    if(lista_parametros == NULL) return 0; // lista vazia
    if(lista_parametros->tipo == NODE_VAR_DECL) return 1; // nó folha
    if(lista_parametros->tipo == NODE_DECL_LISTA){ // nó pai
        return  contar_parametros(lista_parametros->p1) + contar_parametros(lista_parametros->p2);
    }
    return 0;
}

// verificaa se a função foi chamada com o número certo de argumentos
int contar_args(Node *lista_args){
    if(lista_args == NULL) return 0; // lista vazia
    if(lista_args->tipo == NODE_DECL_LISTA){ // nó pai
        return  contar_args(lista_args->p1) + contar_args(lista_args->p2);
    }
    return 1; // não está vazio
}


void percorrer_arvore(Node *node, int usa_retorno) {
    if (node == NULL) return;

    switch (node->tipo) {

        case NODE_VAR_DECL: { // declaração de variável
            TipoDado tipo = ler_tipo(node->p1);

            if (node->p2 == NULL) return;
            char *nome = node->p2->valor.id_val;
            Categoria cat = (node->p3 != NULL) ? VET : VAR;

            if (tipo == TIPO_VOID) {
                registrar_erro(node->linha, "Variavel '%s' declarada como VOID.", nome);
            } 
            else {
                add_simbolo(nome, tipo, cat, node->linha, 0);
            }
            break;
        }

        case NODE_FUN_DECL: { // declaração de função
            TipoDado tipo = ler_tipo(node->p1);
            if (node->p2 == NULL) return;
            char *nome = node->p2->valor.id_val;

            int num_params = contar_parametros(node->p3);
            
            add_simbolo(nome, tipo, FUNC, node->linha, num_params);
            push_escopo(nome);
            // p3 são os parametros
            // p4 é o corpo da função
            percorrer_arvore(node->p3, 0);
            percorrer_arvore(node->p4, 0);
            pop_escopo();
            break;
        }

        case NODE_VAR: { // uso de variável
            if (node->p1 == NULL) return;
            char *nome = node->p1->valor.id_val;

            Simbolo *s = buscar_simbolo(nome);
            if (s == NULL) {
                registrar_erro(node->linha, "Variável '%s' não declarada.", nome);
            } 
            else {
                if (node->p2 != NULL && s->cat != VET) {
                    registrar_erro(node->linha, "Símbolo '%s' não é um vetor.", nome);
                }
            }
            if (node->p2 != NULL) percorrer_arvore(node->p2, 1);
            break;
        }

        case NODE_CALLBACK: { // chamada de função
            if (node->p1 == NULL) return;
            char *nome = node->p1->valor.id_val;

            Simbolo *s = buscar_simbolo(nome);

            // se não encontrou a função
            if (s == NULL) {
                registrar_erro(node->linha, "Função '%s' não declarada.", nome);
            } 
            // se  encontrou, mas não é função
            else if (s->cat != FUNC) {
                registrar_erro(node->linha, "Símbolo '%s' não é uma função.", nome);
            } 
            // verificando o número de argumentos
            else{ 
                int num_args = contar_args(node->p2);
                if (num_args != s->num_params) {
                    registrar_erro(node->linha, "Chamada inválida p/ '%s'. Args esperados: %d, recebidos: %d.", nome, s->num_params, num_args);
                }
                if (s->tipo == TIPO_VOID && usa_retorno == 1) {
                    registrar_erro(node->linha, "Erro: A funcao '%s' retorna VOID e nao pode ser usada em uma expressao.", nome);
                }
                if (s->tipo == TIPO_INT && usa_retorno == 0) {
                    registrar_erro(node->linha, "Chamada invalida. Funcao '%s' retorna valor mas o retorno nao eh usado.", nome);
                }
            }
            percorrer_arvore(node->p2, 1);
            break;
        }

        case NODE_ASSIGN: {
            percorrer_arvore(node->p1, 0); 
            percorrer_arvore(node->p2, 1); 

            // se o lado esquerdo é uma variável, verifica se é vetor sem índice
            if (node->p1->tipo == NODE_VAR) {
                char *nome_var = node->p1->p1->valor.id_val;
                Simbolo *s = buscar_simbolo(nome_var);
                
                // se for vetor e não tiver índice, no caso referencia o vetor e não um elemento
                // vetor != vetor[i]
                if (s != NULL && s->cat == VET && node->p1->p2 == NULL) {
                    registrar_erro(node->linha, "Atribuicao invalida. Vetor '%s' nao pode receber valor escalar (faltou indice?).", nome_var);
                }
            }

            TipoDado tipo_dir = ler_tipo(node->p2); 
            if (tipo_dir == TIPO_VOID) {
                 registrar_erro(node->linha, "Atribuicao invalida. Expressao '%s' retorna VOID.", node->p2->p1->valor.id_val);
            }
            break;
        }   
        
        #pragma region restante dos nós

        case NODE_DECL_LISTA:
        case NODE_COMPOSTO_DECL:
            percorrer_arvore(node->p1, 0);
            percorrer_arvore(node->p2, 0);
            percorrer_arvore(node->p3, 0);
            percorrer_arvore(node->p4, 0);
            break;

        case NODE_IF:
            percorrer_arvore(node->p1, 1); // condição
            percorrer_arvore(node->p2, 0); // if
            percorrer_arvore(node->p3, 0); // else
            break;

        case NODE_WHILE:
            percorrer_arvore(node->p1, 1); // condição
            percorrer_arvore(node->p2, 0); // bloco
            break;
            
        case NODE_RETURN:
            percorrer_arvore(node->p1, 1); // "return" é usada
            break;

        case NODE_SOMA:
        case NODE_MULT:
        case NODE_RELACIONAL:
            percorrer_arvore(node->p1, 1);
            percorrer_arvore(node->p2, 1);
            break;

        case NODE_ID: // nó terminal para representar identificadores apenas 
        case NODE_NUM:
            break;

        default: // para outros tipos de nós de estrutura, percorre os filhos
            percorrer_arvore(node->p1, 0);
            percorrer_arvore(node->p2, 0);
            percorrer_arvore(node->p3, 0);
            percorrer_arvore(node->p4, 0);
            break;

        #pragma endregion    
    }
}

#pragma endregion


int analise_semantica(Node *raiz_ast) {
    n_erros = 0;
    printf( "\nIniciando Analise Semantica...\n");

    // 1 - inicia a pilha de escopos
    push_escopo("GLOBAL"); 
    
    // 2 - inserir funções padrão de C-
    add_simbolo("input", TIPO_INT, FUNC, 0, 0);
    add_simbolo("output", TIPO_VOID, FUNC, 0, 1);

    // 3 - popular a pilha com os símbolos da árvore de analise sintatica
    percorrer_arvore(raiz_ast, 0);
    
    // uma verificação se tem função main
    if (buscar_simbolo("main") == NULL) {
        registrar_erro(0, "Funcao 'main' nao declarada.");
    }
 
    // 4 - libera a pilha e os símbolos
    pop_escopo();
    
    // 5 - imprimir resultados
    if (n_erros == 0){
        // imprime a árvore pelo parser
        printf( "Analise Semantica Concluida com Sucesso.\n");
        print_tabela();
    }    
    else {
        printf( "Analise Semantica Concluida com %d erros.\n", n_erros);
        imprimir_erros();
    }    
    
    // 6 - libera a tabela de símbolos
    free_tabela();
    return n_erros;
}

