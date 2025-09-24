%option noyywrap

DIGIT [0-9]
%%
[1-9]{DIGIT}* printf("DEC");
0[0-7]* printf("OCT");
0x[0-9A-Fa-f]+ printf("HEX");
0b[01]+ printf("BIN");
<<EOF>> return 0;
%%
int main(int argc, char *argv[])
{
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
