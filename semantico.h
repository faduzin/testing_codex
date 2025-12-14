#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "tabela_simbolos.h"

extern int errosSemanticos;

void iniciarAnaliseSemantica(void);
void registrarDeclaracaoIdentificador(const char *nome, TipoSimbolo tipoSimbolo, TipoDado tipoDado, int linha, int tamanho);
void registrarUsoIdentificador(const char *nome, int linha);
void entrarEscopoFuncao(const char *nome);
void entrarEscopoBloco(void);
void sairEscopoAtual(void);

#endif
