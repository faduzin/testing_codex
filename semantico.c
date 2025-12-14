#include "semantico.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int errosSemanticos = 0;
static int contadorEscopos = 0;

void iniciarAnaliseSemantica(void) {
    inicializarTabelaSimbolos();
    errosSemanticos = 0;
}

void registrarDeclaracaoIdentificador(const char *nome, TipoSimbolo tipoSimbolo, TipoDado tipoDado, int linha, int tamanho) {
    const char *escopo = escopoAtual();
    if (buscarSimbolo(nome, escopo)) {
        printf("ERRO SEMÂNTICO: %s LINHA: %d\n", nome, linha);
        errosSemanticos++;
        return;
    }
    inserirSimbolo(nome, tipoSimbolo, tipoDado, escopo, linha, tamanho);
}

void registrarUsoIdentificador(const char *nome, int linha) {
    EntradaSimbolo *entrada = buscarEmEscopos(nome);
    if (!entrada) {
        printf("ERRO SEMÂNTICO: %s LINHA: %d\n", nome, linha);
        errosSemanticos++;
        return;
    }
    registrarUsoSimbolo(entrada, linha);
}

void entrarEscopoFuncao(const char *nome) {
    empilharEscopo(nome);
}

void entrarEscopoBloco(void) {
    char nomeEscopo[64];
    snprintf(nomeEscopo, sizeof(nomeEscopo), "bloco_%d", contadorEscopos++);
    empilharEscopo(nomeEscopo);
}

void sairEscopoAtual(void) {
    desempilharEscopo();
}
