# Nome do compilador
CC=gcc
# O nome do seu executável final
EXECUTAVEL=parser

# Arquivos fonte gerados e o seu código
FONTES=parser.tab.c lex.yy.c arvore.c

# Target principal: o que 'make' faz por padrão
all: $(EXECUTAVEL)

# Target 'run': compila e executa
run: $(EXECUTAVEL)
	./$(EXECUTAVEL) sort.txt

# Regra para criar o executável:
# Ele depende dos arquivos listados em FONTES
$(EXECUTAVEL): $(FONTES)
	$(CC) -o $(EXECUTAVEL) $(FONTES)

# Regra para criar o parser.tab.c (do Bison)
# Ele depende do parser.y
parser.tab.c: parser.y
	bison -d parser.y

# Regra para criar o lex.yy.c (do Flex)
# Ele depende do scanner.l
lex.yy.c: scanner.l
	flex scanner.l

# Target 'clean': apaga os arquivos gerados para limpar o diretório
clean:
	rm -f $(EXECUTAVEL) parser.tab.c parser.tab.h lex.yy.c