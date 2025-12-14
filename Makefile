BISON=bison -d -o cmenos.tab.c analisador_sintatico.y
FLEX=flex -o lex.yy.c analisador_lexico.l
CC=gcc
CFLAGS=-Wall -Wextra -std=c11
OBJ=ast.o tabela_simbolos.o semantico.o cmenos.tab.o lex.yy.o main.o

all: compilador

cmenos.tab.c cmenos.tab.h: analisador_sintatico.y
	$(BISON)

lex.yy.c: analisador_lexico.l cmenos.tab.h
	$(FLEX)

compilador: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) -lfl

ast.o: ast.c ast.h

semantico.o: semantico.c semantico.h tabela_simbolos.h ast.h

tabela_simbolos.o: tabela_simbolos.c tabela_simbolos.h ast.h

cmenos.tab.o: cmenos.tab.c ast.h semantico.h

lex.yy.o: lex.yy.c cmenos.tab.h

main.o: main.c ast.h semantico.h

clean:
	rm -f $(OBJ) lex.yy.c cmenos.tab.c cmenos.tab.h compilador

.PHONY: all clean
