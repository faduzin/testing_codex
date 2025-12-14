#ifndef AST_H
#define AST_H

#include <stdio.h>

typedef enum {
    NO_PROGRAMA,
    NO_DECL_VAR,
    NO_DECL_FUN,
    NO_PARAM,
    NO_BLOCO,
    NO_ATRIBUICAO,
    NO_OPERACAO_REL,
    NO_OPERACAO_SOMA,
    NO_OPERACAO_MULT,
    NO_ATIVACAO,
    NO_RETORNO,
    NO_SELECAO,
    NO_ITERACAO,
    NO_NUMERO,
    NO_IDENTIFICADOR,
    NO_VETOR,
    NO_EXPLICITO
} TipoNo;

typedef enum {
    TIPO_DADO_INDEFINIDO = 0,
    TIPO_DADO_INT,
    TIPO_DADO_VOID
} TipoDado;

typedef struct ast_no {
    TipoNo tipo;
    TipoDado tipoDado;
    char *lexema;
    int valor;
    int linha;
    struct ast_no *filhos[3];
    struct ast_no *irmao;
} ASTNo;

ASTNo *criarNoAST(TipoNo tipo, const char *lexema, int valor, int linha);
void definirTipoNo(ASTNo *no, TipoDado tipo);
void adicionarFilho(ASTNo *pai, ASTNo *filho, int indice);
void anexarIrmao(ASTNo *no, ASTNo *irmao);
void imprimirAST(const ASTNo *no, int nivel);

#endif
