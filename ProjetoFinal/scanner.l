%option noyywrap
%option yylineno
%{
#include "arvore.h"
#include "parser.tab.h"
#include<string.h>
int erro_lexico = 0;
%}

DIGITO          [0-9]             
LETRA           [a-zA-Z]
ID              {LETRA}{LETRA}*
NUM             {DIGITO}{DIGITO}*

PLUS            \+
MINUS           \-
MULT            \*
DIV             \/

%%

"int"                       { yylval.no = criar_folha_id(strdup("int"),yylineno); return INT; }
"void"                      { yylval.no = criar_folha_id(strdup("void"),yylineno); return VOID; }

"return"                    { yylval.no = NULL; return RETURN; }
"while"                     { yylval.no = NULL; return WHILE; }
"if"                        { yylval.no = NULL; return IF; }
"else"                      { yylval.no = NULL; return ELSE; }

{PLUS}                      { yylval.no = criar_folha_id(strdup(yytext),yylineno); yylval.no->tipo = NODE_SOMA; return PLUS; }
{MINUS}                     { yylval.no = criar_folha_id(strdup(yytext),yylineno); yylval.no->tipo = NODE_SOMA; return MINUS; }
{MULT}                      { yylval.no = criar_folha_id(strdup(yytext),yylineno); yylval.no->tipo = NODE_MULT; return MULT; }
{DIV}                       { yylval.no = criar_folha_id(strdup(yytext),yylineno); yylval.no->tipo = NODE_MULT; return DIV; }
"<"|"<="|">"|">="|"=="|"!=" { yylval.no = criar_folha_id(strdup(yytext),yylineno); yylval.no->tipo = NODE_RELACIONAL; return RELACIONAL; }

{NUM}                       { yylval.no = criar_folha_num(atoi(yytext)); return NUM; }
{ID}                        { yylval.no = criar_folha_id(strdup(yytext), yylineno); return ID; }

"("                         { yylval.no = NULL; return OPN_PAR; }
")"                         { yylval.no = NULL; return CLS_PAR; }
"["                         { yylval.no = NULL; return OPN_CLC; }
"]"                         { yylval.no = NULL; return CLS_CLC; }
"{"                         { yylval.no = NULL; return OPN_CHA; }
"}"                         { yylval.no = NULL; return CLS_CHA; }
";"                         { yylval.no = NULL; return PNT_VRG; }
","                         { yylval.no = NULL; return VRG; }
"="                         { yylval.no = NULL; return EQ; }

"/*"([^*]|\*+[^*/])*"\*/"   { }
[ \t\n\r]+
[[:space:]]+ 


[^ \t\n\r()\[\]{};,=+*\/<>=!-]+ { /* Qualquer outra coisa */
    erro_lexico = 1;
    fprintf(stderr, "Erro Lexico (Linha %d): Lexema invalido '%s'\n", yylineno, yytext);
}

%% 

/*
int main(int argc, char *argv[]){
    FILE *f_in;
    if (argc == 2)
    {
        if(f_in = fopen(argv[1],"r")) yyin = f_in;
        else perror(argv[0]);
    }
    else yyin = stdin;
    
    yylex();

    fprintf(stderr, "\nCódigo nos conformes com a linguagem C-.\n");

    return(0);
}
*/