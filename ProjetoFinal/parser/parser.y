%{
#include<stdio.h>
#include<stdlib.h>

int yyparser(void);
int yylex(void);
int yywrap(){
    return 1;
}
void yyerror(const char *s);
%}

/*   Tokens   */

%token ID NUM 
%token OPN_PAR CLS_PAR OPN_CLC CLS_CLC OPN_CHA CLS_CHA PNT_VRG VRG EQ
%token RETURN WHILE INT VOID
%token SOMA MULT RELACIONAL 
%right IF ELSE

%start programa
/*     Regras da Gramática     */

%%

programa: declaracao-lista;
declaracao-lista: declaracao-lista declaracao | declaracao;
declaracao: var-declaracao | fun-declaracao;
var-declaracao: tipo-especificador ID PNT_VRG | tipo-especificador ID OPN_CLC NUM CLS_CLC PNT_VRG;
tipo-especificador: INT | VOID;
fun-declaracao: tipo-especificador ID OPN_PAR params CLS_PAR composto-decl;
params: param-lista | VOID;
param-lista: param-lista VRG param | param;
param: tipo-especificador ID | tipo-especificador ID OPN_CLC CLS_CLC;
composto-decl: OPN_CHA local-declaracoes statement-lista CLS_CHA;
local-declaracoes: local-declaracoes var-declaracao | ;
statement-lista: statement-lista statement | ;
statement: expressao-decl | composto-decl | selecao-decl | iteracao-decl | retorno-decl;
expressao-decl: expressao PNT_VRG | PNT_VRG ;
selecao-decl: IF OPN_PAR expressao CLS_PAR statement | IF OPN_PAR expressao CLS_PAR statement ELSE statement;
iteracao-decl: WHILE OPN_PAR expressao CLS_PAR statement;
retorno-decl: RETURN PNT_VRG | RETURN expressao PNT_VRG;
expressao: var EQ expressao | simples-expressao;
var: ID | ID OPN_CLC expressao CLS_CLC;
simples-expressao: soma-expressao relacional soma-expressao | soma-expressao;
relacional: RELACIONAL;

soma-expressao: soma-expressao soma termo | termo;
soma: SOMA; 

termo: termo mult fator | fator;
mult: MULT;

fator: OPN_PAR expressao CLS_PAR | var | ativacao | NUM;
ativacao: ID OPN_PAR args CLS_PAR;
args: arg-lista | ;
arg-lista: arg-lista VRG expressao | expressao;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro de sintaxe: %s\n", s);
}