%{
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "arvore.h"
#include "semantic.h"

int yyparser(void);
int yylex(void);
int yywrap(){
    return 1;
}
void yyerror(const char *s);
Node *raiz_ast = NULL;
extern FILE *yyin;
extern int yylineno;

extern int erro_lexico;
%}



%union {
    struct Node *no; /* Todos os valores serão ponteiros para Node */
}

%define parse.error verbose

/*   Tokens   */

%token <no> ID NUM 
%token <no> INT VOID
%token <no> PLUS MINUS MULT DIV RELACIONAL 

%token OPN_PAR CLS_PAR OPN_CLC CLS_CLC OPN_CHA CLS_CHA PNT_VRG VRG EQ
%token RETURN WHILE
%right IF ELSE

%type <no> programa declaracao-lista declaracao var-declaracao tipo-especificador
%type <no> fun-declaracao params param-lista param composto-decl
%type <no> local-declaracoes statement-lista statement expressao-decl
%type <no> selecao-decl iteracao-decl retorno-decl expressao var
%type <no> simples-expressao relacional soma-expressao soma termo
%type <no> mult fator ativacao args arg-lista

%start programa

%%

programa:           declaracao-lista { raiz_ast = $1; $$ = $1; };

declaracao-lista:   declaracao-lista declaracao { $$ = criar_no(NODE_DECL_LISTA, $1, $2, NULL, NULL); } | 
                    declaracao { $$ = $1; };

declaracao:         var-declaracao { $$ = $1; } | 
                    fun-declaracao { $$ = $1; };

var-declaracao:     tipo-especificador ID PNT_VRG { $$ = criar_no(NODE_VAR_DECL, $1, $2, NULL, NULL); }| 
                    tipo-especificador ID OPN_CLC NUM CLS_CLC PNT_VRG { $$ = criar_no(NODE_VAR_DECL, $1, $2, $4, NULL); };

tipo-especificador: INT { $$ = $1; }| 
                    VOID { $$ = $1;};

fun-declaracao:     tipo-especificador ID OPN_PAR params CLS_PAR composto-decl { $$ = criar_no(NODE_FUN_DECL, $1, $2, $4, $6);} ;

params:             param-lista { $$ = $1; }| 
                    VOID { $$ = $1; };

param-lista:        param-lista VRG param { $$ = criar_no(NODE_DECL_LISTA, $1, $3, NULL, NULL);} | 
                    param { $$ = $1; };

param:              tipo-especificador ID { $$ = criar_no(NODE_VAR_DECL, $1, $2, NULL, NULL); }| 
                    tipo-especificador ID OPN_CLC CLS_CLC { $$ = criar_no(NODE_VAR_DECL, $1, $2, criar_folha_id(strdup("[]"), yylineno), NULL); };

composto-decl:      OPN_CHA local-declaracoes statement-lista CLS_CHA { $$ = criar_no(NODE_COMPOSTO_DECL, $2, $3, NULL, NULL);};

local-declaracoes:  local-declaracoes var-declaracao { $$ = criar_no(NODE_DECL_LISTA, $1, $2, NULL, NULL); }| 
                    { $$ = NULL; };

statement-lista:    statement-lista statement { $$ = criar_no(NODE_DECL_LISTA, $1, $2, NULL, NULL); }| 
                    { $$ = NULL; };

statement:          expressao-decl { $$ = $1; } | 
                    composto-decl { $$ = $1; }| 
                    selecao-decl { $$ = $1; }| 
                    iteracao-decl { $$ = $1; }| 
                    retorno-decl{ $$ = $1; };

expressao-decl:     expressao PNT_VRG { $$ = $1; }| 
                    PNT_VRG { $$ = NULL; };

selecao-decl:       IF OPN_PAR expressao CLS_PAR statement { $$ = criar_no(NODE_IF, $3, $5, NULL, NULL); }| 
                    IF OPN_PAR expressao CLS_PAR statement ELSE statement { $$ = criar_no(NODE_IF, $3, $5, $7, NULL);};

iteracao-decl:      WHILE OPN_PAR expressao CLS_PAR statement { $$ = criar_no(NODE_WHILE, $3, $5, NULL, NULL);};

retorno-decl:       RETURN PNT_VRG { $$ = criar_no(NODE_RETURN, NULL, NULL, NULL, NULL); }| 
                    RETURN expressao PNT_VRG { $$ = criar_no(NODE_RETURN, $2, NULL, NULL, NULL);};

expressao:          var EQ expressao { $$ = criar_no(NODE_ASSIGN, $1, $3, NULL, NULL);}| 
                    simples-expressao { $$ = $1; };

var:                ID { $$ = criar_no(NODE_VAR, $1, NULL, NULL, NULL); } | 
                    ID OPN_CLC expressao CLS_CLC { $$ = criar_no(NODE_VAR, $1, $3, NULL, NULL);};

simples-expressao:  soma-expressao relacional soma-expressao 
                    {
                    /* $2 é o nó do operador (<, ==, etc.). Vamos "promovê-lo" a pai */
                    $2->p1 = $1; /* p1=lado esq */
                    $2->p2 = $3; /* p2=lado dir */
                    $$ = $2;
                    } 
                  | soma-expressao { $$ = $1; };

relacional:         RELACIONAL { $$ = $1; };

soma-expressao:     soma-expressao soma termo 
                    {
                    /* $2 é o nó do operador (+ ou -). Promove a pai */
                    $2->p1 = $1;
                    $2->p2 = $3;
                    $$ = $2;
                    }
                  | termo { $$ = $1; };

soma:               PLUS { $$ = $1; }| 
                    MINUS { $$ = $1; }; 

termo:              termo mult fator 
                    {
                    /* $2 é o nó do operador (* ou /). Promove a pai */
                    $2->p1 = $1;
                    $2->p2 = $3;
                    $$ = $2;
                    }
                  | fator { $$ = $1; };

mult:               MULT { $$ = $1; } | 
                    DIV { $$ = $1; };

fator:              OPN_PAR expressao CLS_PAR { $$ = $2; }| 
                    var { $$ = $1; }| 
                    ativacao { $$ = $1; }| 
                    NUM { $$ = $1; };

ativacao:           ID OPN_PAR args CLS_PAR { $$ = criar_no(NODE_CALLBACK, $1, $3, NULL, NULL);} ;

args:               arg-lista { $$ = $1; }| 
                    { $$ = NULL; };

arg-lista:          arg-lista VRG expressao { $$ = criar_no(NODE_DECL_LISTA, $1, $3, NULL, NULL);} | 
                    expressao { $$ = $1; };

%%


int main(int argc, char *argv[]){
    FILE *f_in;
if (argc == 2){
        if(!(f_in = fopen(argv[1],"r"))) {
            perror(argv[0]);
            return 1;
        }
        yyin = f_in;
    }
    else {
        yyin = stdin;
    }

    int resultado_parse = yyparse();

    if (resultado_parse == 0 && erro_lexico == 0) {
        if (raiz_ast != NULL) {

            int erros = analise_semantica(raiz_ast);
            if(erros == 0){

                printf("\nCódigo sem erros léxicos, sintáticos ou semânticos!\n");

                printf("\nÁrvore Sintática Gerada com Sucesso.\n");
                print_arvore(raiz_ast, 0);
            }
            else{
                printf("\nArvore suprimida devido a erros semanticos.\n");
            }

            free_tree(raiz_ast);
        }
    } 
    else {

        fprintf(stderr, "\nCompilacao abortada devido a erros.\n");
    }

    if (f_in) fclose(f_in);

    return(0);
}

void yyerror(const char *s) {
    fprintf(stderr, "Erro Sintatico (Linha %d): %s\n", yylineno, s);
}