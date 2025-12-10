# Nome do compilador
CC=gcc

# Flags de compilação (Opcional, mas recomendado para ver erros)
# -g: gera informações de debug (útil para gdb/valgrind)
# -Wall: mostra todos os avisos (warnings)
CFLAGS=-g -Wall

# O nome do seu executável final
EXECUTAVEL=parser

# Arquivos fonte (ADICIONADO: semantic.c)
FONTES=parser.tab.c lex.yy.c arvore.c semantic.c

# Target principal
all: $(EXECUTAVEL)

# Target 'run': compila e executa
run: $(EXECUTAVEL)
	./$(EXECUTAVEL) sort_test.txt

# Regra para criar o executável
# O '$@' representa o nome do target (parser)
# O '$^' representa todas as dependências (os fontes)
$(EXECUTAVEL): $(FONTES)
	$(CC) $(CFLAGS) -o $@ $^

# Regra do Bison
# Gera tanto o .c quanto o .h
parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

# Regra do Flex
# Ele depende do parser.tab.h porque o scanner.l faz #include "parser.tab.h"
lex.yy.c: scanner.l parser.tab.h
	flex scanner.l

# Target 'clean'
clean:
	rm -f $(EXECUTAVEL) parser.tab.c parser.tab.h lex.yy.c