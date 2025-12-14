#ifndef TABELA_SIMBOLOS_H
#define TABELA_SIMBOLOS_H

#include "ast.h"

typedef enum {
    SIMBOLO_VARIAVEL,
    SIMBOLO_FUNCAO,
    SIMBOLO_VETOR,
    SIMBOLO_PARAMETRO
} TipoSimbolo;

typedef struct linha_lista {
    int linha;
    struct linha_lista *proximo;
} ListaLinha;

typedef struct entrada_simbolo {
    char *nome;
    TipoSimbolo tipoSimbolo;
    TipoDado tipoDado;
    char *escopo;
    int tamanho;
    int linhaDeclaracao;
    ListaLinha *usos;
    struct entrada_simbolo *proximo;
} EntradaSimbolo;

void inicializarTabelaSimbolos(void);
EntradaSimbolo *buscarSimbolo(const char *nome, const char *escopo);
EntradaSimbolo *buscarEmEscopos(const char *nome);
EntradaSimbolo *inserirSimbolo(const char *nome, TipoSimbolo tipoSimbolo, TipoDado tipoDado, const char *escopo, int linha, int tamanho);
void registrarUsoSimbolo(EntradaSimbolo *entrada, int linha);
void imprimirTabelaSimbolos(void);
void limparTabelaSimbolos(void);

const char *escopoAtual(void);
void empilharEscopo(const char *nome);
void desempilharEscopo(void);

#endif
