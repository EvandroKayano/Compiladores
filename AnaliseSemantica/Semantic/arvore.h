#ifndef ARVORE_H
#define ARVORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    NODE_PROGRAMA,
    NODE_DECL_LISTA,
    NODE_VAR_DECL,
    NODE_FUN_DECL,
    NODE_COMPOSTO_DECL,
    NODE_IF,
    NODE_WHILE,
    NODE_RETURN,
    NODE_ASSIGN,
    NODE_RELACIONAL,
    NODE_SOMA,
    NODE_MULT,
    NODE_VAR,
    NODE_CALLBACK,
    NODE_ID,
    NODE_NUM
} Tipo_No;

typedef struct Node {
    Tipo_No tipo;
    union{
        int num_val;
        char *id_val;
    }valor;

    struct Node *p1;
    struct Node *p2;
    struct Node *p3;
    struct Node *p4;

    int linha;
} Node;

Node *criar_no(Tipo_No tipo, Node *p1, Node *p2, Node *p3, Node *p4);
Node *criar_folha_num(int valor);
Node *criar_folha_id(char *valor, int linha);
void print_arvore(Node *raiz, int level);
void free_tree(Node *raiz);

#endif