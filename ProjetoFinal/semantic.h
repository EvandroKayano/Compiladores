#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "arvore.h"

#define HASH_SIZE 211
typedef enum { TIPO_INT, TIPO_VOID, TIPO_ERRO } TipoDado;
typedef enum{ VAR, FUNC, VET } Categoria;

// item a ser aderido à hash map
typedef struct Simbolo{
    char *nome;
    TipoDado tipo;
    Categoria cat;
    int linha;
    int num_params;

    struct Simbolo *prox;
}Simbolo;

// pilha das areas do código -> global, funções, blocos de estrutura (while, if)
typedef struct Pilha{
    Simbolo *hashes[HASH_SIZE];
    struct Pilha *pai;
    int nivel;
    // para coluna na tabela de símbolos 
    char *nome_escopo;
}Pilha;

typedef struct TabelaSimbolos {
    int id_entrada;      // Número da entrada (1, 2, 3...)
    char *nome;          // Nome do ID
    char *escopo;        // Nome do Escopo onde foi criado
    char *cat;           // "var", "fun", "param"
    char *tipo;          // "int", "void"
    int linha;           // Linha de declaração
    int hash_index;      // Índice na tabela hash
    struct TabelaSimbolos *prox;
} TabelaSimbolos;

typedef struct MensagemErro{
    char mensagem[256];
    int linha;

    struct MensagemErro *prox;
} MensagemErro;

void push_escopo(char *nome);

int analise_semantica(Node *raiz_ast);

#endif