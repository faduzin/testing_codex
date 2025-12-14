#include "ast.h"
#include <stdlib.h>
#include <string.h>

static char *copiarTexto(const char *origem) {
    if (!origem) return NULL;
    size_t tamanho = strlen(origem) + 1;
    char *destino = (char *)malloc(tamanho);
    if (!destino) return NULL;
    memcpy(destino, origem, tamanho);
    return destino;
}

ASTNo *criarNoAST(TipoNo tipo, const char *lexema, int valor, int linha) {
    ASTNo *novo = (ASTNo *)malloc(sizeof(ASTNo));
    if (!novo) {
        fprintf(stderr, "Falha ao alocar nó da AST\n");
        exit(1);
    }
    novo->tipo = tipo;
    novo->tipoDado = TIPO_DADO_INDEFINIDO;
    novo->lexema = copiarTexto(lexema);
    novo->valor = valor;
    novo->linha = linha;
    novo->irmao = NULL;
    for (int i = 0; i < 3; i++) novo->filhos[i] = NULL;
    return novo;
}

void definirTipoNo(ASTNo *no, TipoDado tipo) {
    if (no) {
        no->tipoDado = tipo;
    }
}

void adicionarFilho(ASTNo *pai, ASTNo *filho, int indice) {
    if (pai && indice >= 0 && indice < 3) {
        pai->filhos[indice] = filho;
    }
}

void anexarIrmao(ASTNo *no, ASTNo *irmao) {
    if (!no) return;
    ASTNo *atual = no;
    while (atual->irmao) {
        atual = atual->irmao;
    }
    atual->irmao = irmao;
}

static const char *nomeTipoNo(TipoNo tipo) {
    switch (tipo) {
        case NO_PROGRAMA: return "programa";
        case NO_DECL_VAR: return "declaracao_var";
        case NO_DECL_FUN: return "declaracao_funcao";
        case NO_PARAM: return "parametro";
        case NO_BLOCO: return "bloco";
        case NO_ATRIBUICAO: return "atribuicao";
        case NO_OPERACAO_REL: return "operacao_rel";
        case NO_OPERACAO_SOMA: return "operacao_soma";
        case NO_OPERACAO_MULT: return "operacao_mult";
        case NO_ATIVACAO: return "ativacao";
        case NO_RETORNO: return "retorno";
        case NO_SELECAO: return "selecao";
        case NO_ITERACAO: return "iteracao";
        case NO_NUMERO: return "numero";
        case NO_IDENTIFICADOR: return "identificador";
        case NO_VETOR: return "vetor";
        case NO_EXPLICITO: return "expressao";
        default: return "desconhecido";
    }
}

static const char *nomeTipoDado(TipoDado tipo) {
    switch (tipo) {
        case TIPO_DADO_INT: return "int";
        case TIPO_DADO_VOID: return "void";
        default: return "?";
    }
}

static void imprimirIndentacao(int nivel) {
    for (int i = 0; i < nivel; i++) {
        printf("  ");
    }
}

void imprimirAST(const ASTNo *no, int nivel) {
    const ASTNo *atual = no;
    while (atual) {
        imprimirIndentacao(nivel);
        printf("%s", nomeTipoNo(atual->tipo));
        if (atual->lexema) {
            printf(" (%s)", atual->lexema);
        }
        if (atual->tipoDado != TIPO_DADO_INDEFINIDO) {
            printf(" :%s", nomeTipoDado(atual->tipoDado));
        }
        if (atual->valor) {
            printf(" valor=%d", atual->valor);
        }
        if (atual->linha) {
            printf(" [linha %d]", atual->linha);
        }
        printf("\n");

        for (int i = 0; i < 3; i++) {
            if (atual->filhos[i]) {
                imprimirAST(atual->filhos[i], nivel + 1);
            }
        }
        atual = atual->irmao;
    }
}
