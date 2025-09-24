%option noyywrap


DIGITO          [0-9]             
LETRA           [a-zA-Z]
ID              {LETRA}{LETRA}*
NUM             {DIGITO}{DIGITO}*

RESERVADA       else|if|int|return|void|while

OPERACOES       \+|\-|\*|\/
COMPARADORES    (<|<=|>|>=|==|!=)

SIMBOLO {OPERACOES}|{COMPARADORES}|\=|\;|\,|\(|\)|\[|\]|\{|\}

COMENTARIO "/*"([^*]|\*+[^*/])*"\*/"

%%

{COMENTARIO}                printf("%s", yytext);
{RESERVADA}                 printf("RESERVADA ");
{SIMBOLO}                   printf("SÍMBOLO ");
{LETRA}{LETRA}*             printf("ID ");
{DIGITO}{DIGITO}*           printf("NUM ");

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
    return(0);
}