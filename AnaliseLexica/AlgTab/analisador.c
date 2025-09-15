#include<stdio.h>
#include<string.h>
#include<ctype.h>

int is_keyword(char *word);
int t_char(char ch);

int main(){
    // VARIAVEIS E AUXILIARES
    FILE *fp, *temp, *output;
    char string[100], ch, xh;
    int cont = 0, nReservadas = 0;
    char reservadas[50][50];

    // VARIAVEIS DO ALGORITMO
    int erro = 0, estado = 1;

    /* "Dado um caractere e um estado, para onde ele vai? e ainda é aceitável a string?"

                    letra   digito  outro
        estado 1    2v      0       0
        estado 2    2v      2v      3f
        estado 3    0       0       0

    */
    
    int tabela[3][3] = {
        {2,0,0},
        {2,2,3},
        {0,0,0}
    };

    fp = fopen("sort.txt", "r");
    temp = fopen("output.txt", "w+");
    output = fopen("output.txt", "w");

    // primeiro substituir a palavras reservadas pelo numero 0

    while ((ch = fgetc(fp)) != EOF) {              // lê caractere à caractere
        if(isalpha(ch)){           // SE FOR LETRA
            if(cont < (int)sizeof(string)-1) {  // para não passa do tamanho da string
                string[cont++] = (char)ch;      // guarda o caractere
            }

        }
        else{                                   // QUALQUER  CARACTERE  
            if(cont > 0){       
                string[cont] = '\0';            

                if(is_keyword(string))          // verifica se é palavra reservada  
                    fprintf(temp, "%s", string);
                else{                           // se não for
                    fprintf(temp, "`", string);
                    strcpy(reservadas[nReservadas++], string); // guardo a palavra reservada para reescrita
                }    
                cont = 0;
            }
            fputc(ch, temp);     // escreve o \0, espaço, \n , \, , etc...
        }
    }
  
    // reposicionar para o começo do .txt
    fflush(temp);
    rewind(temp);

    // segundo escrever o código substituindo identificadores por ID

    nReservadas = 0;
    cont = 0;

    while((xh = fgetc(temp)) != EOF){

        if( xh == '`' ){    //  PALAVRA RESERVADA
            fprintf(output, "%s", reservadas[nReservadas++]);
            estado = 1;
        }
        else{               //  ALGORITMO DIRIGIDO POR TABELA
        /*  
            erro é se a palavra ainda atende o padrão
            estado representa o DFA assim como no slide, estado 3 é aceitação
        */
            int tipo = t_char(xh);
            int novoEstado = tabela[estado-1][tipo];

            if(!isalpha(xh)) erro = 1; // se o primeiro caractere não for letra, não é id
            
            if(!erro){  // válido
                
                estado = novoEstado; // avança
                if(estado == 2)
                    if(cont < (int)sizeof(string)-1)   
                        string[cont++] = (char)xh;      
            }
            else{       // inválido
                if(tipo == 1 && novoEstado == 2){      // numero
                    if(cont < (int)sizeof(string)-1)   
                        string[cont++] = (char)xh; 
                    erro = 0;   
                }
                else if(tipo == 2 && novoEstado == 3){ // outro
                    string[cont] = '\0';
                    fprintf(output,"ID");
                    fputc(xh, output);
                    cont = 0;
                    estado = 1;
                    erro = 0;
                }
                else{ // começa por outro caractere
                    fputc(xh, output);
                    estado = 1;
                    erro = 0;
                }
            }    
        }
    }

    fclose(fp);
    fclose(temp);
    fclose(output);
}

int is_keyword(char *word){
    if( !strcmp(word, "int")     ||
        !strcmp(word, "void")    ||
        !strcmp(word, "while")   ||
        !strcmp(word, "main")    ||
        !strcmp(word, "return")
    ){
        return 0;
    }
    return 1;
}

int t_char(char ch){
    if(isalpha(ch)) return 0;
    else if(isdigit(ch)) return 1;
    else return 2;
}

/*
    Resumo do analisador léxico

    While 1:
    Identifica palavras reservadas, substitui ela pelo acento agudo '`' e guardas elas num array de strings "reservadas". 
    O restante do código, e inclusive os espaços e tabulações, são escritos no temp.txt .

    While 2:
    Ao identificar o acento agudo, escreve a palavra reservada na ordem que aparece.
    Para cada caractere lido de temp.txt, verificamos se é uma letra, número ou outro caractere.
    Caso seja uma letra, guardamos num array temporário.

    Se for um número, temos 2 situações:
        1 - Começa por número: Logo não é um identificador, então apenas escrevemos no output;
        2 - Começa por letra e aparece um número: guardamos no array.

    Se for um caractere qualquer, também temos 2 situações:
        1 - Começa por este caractere: Não é identificador, então apenas escrevemos no arquivo;
        2 - O caractere anterior era letra ou número: ele descarta a string, substitui por "ID" e escreve o caractere/espaço/tabulação

*/