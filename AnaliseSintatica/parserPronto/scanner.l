%option noyywrap
%{
#include "parser.tab.h"
%}

DIGITO          [0-9]             
LETRA           [a-zA-Z]
ID              {LETRA}{LETRA}*
NUM             {DIGITO}{DIGITO}*

SOMA            \+|\-
MULT            \*|\/

%%


"int"                       return INT;
"void"                      return VOID;
"return"                    return RETURN;
"while"                     return WHILE;
"if"                        return IF;
"else"                      return ELSE;

{NUM}                       return NUM;
{ID}                        return ID;

{SOMA}                      return SOMA;
{MULT}                      return MULT;

"<"|"<="|">"|">="|"=="|"!=" return RELACIONAL;

"("                         return OPN_PAR;
")"                         return CLS_PAR;
"["                         return OPN_CLC;
"]"                         return CLS_CLC;
"{"                         return OPN_CHA;
"}"                         return CLS_CHA;
";"                         return PNT_VRG;
","                         return VRG;
"="                         return EQ;

"/*"([^*]|\*+[^*/])*"\*/" { }
[ \t\n]+
[[:space:]]+ /*Funciona OK?*/

. printf("Caractere não pertence à linguagem C-: '%s'\n", yytext);

%% 

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