#include "tabela_simbolos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_TABELA 211
#define TAMANHO_MAX_ESCOPOS 128

static EntradaSimbolo *tabela[TAMANHO_TABELA];
static char *pilhaEscopos[TAMANHO_MAX_ESCOPOS];
static int topoEscopos = -1;

static int hash(const char *chave) {
    int h = 0;
    while (*chave) {
        h = (h << 4) + *chave++;
        int g = h & 0xF0000000;
        if (g) h ^= g >> 24;
        h &= ~g;
    }
    return h % TAMANHO_TABELA;
}

void inicializarTabelaSimbolos(void) {
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        tabela[i] = NULL;
    }
    topoEscopos = -1;
    empilharEscopo("global");
}

static char *duplicarTexto(const char *texto) {
    if (!texto) return NULL;
    size_t tam = strlen(texto) + 1;
    char *copia = (char *)malloc(tam);
    if (!copia) return NULL;
    memcpy(copia, texto, tam);
    return copia;
}

EntradaSimbolo *buscarSimbolo(const char *nome, const char *escopo) {
    int h = hash(nome);
    EntradaSimbolo *atual = tabela[h];
    while (atual) {
        if (strcmp(atual->nome, nome) == 0 && strcmp(atual->escopo, escopo) == 0) {
            return atual;
        }
        atual = atual->proximo;
    }
    return NULL;
}

EntradaSimbolo *buscarEmEscopos(const char *nome) {
    for (int i = topoEscopos; i >= 0; i--) {
        EntradaSimbolo *encontrado = buscarSimbolo(nome, pilhaEscopos[i]);
        if (encontrado) return encontrado;
    }
    return NULL;
}

EntradaSimbolo *inserirSimbolo(const char *nome, TipoSimbolo tipoSimbolo, TipoDado tipoDado, const char *escopo, int linha, int tamanho) {
    int h = hash(nome);
    EntradaSimbolo *nova = (EntradaSimbolo *)malloc(sizeof(EntradaSimbolo));
    nova->nome = duplicarTexto(nome);
    nova->tipoSimbolo = tipoSimbolo;
    nova->tipoDado = tipoDado;
    nova->escopo = duplicarTexto(escopo);
    nova->tamanho = tamanho;
    nova->linhaDeclaracao = linha;
    nova->usos = NULL;
    nova->proximo = tabela[h];
    tabela[h] = nova;
    return nova;
}

void registrarUsoSimbolo(EntradaSimbolo *entrada, int linha) {
    if (!entrada) return;
    ListaLinha *novoUso = (ListaLinha *)malloc(sizeof(ListaLinha));
    novoUso->linha = linha;
    novoUso->proximo = entrada->usos;
    entrada->usos = novoUso;
}

static const char *nomeTipoSimbolo(TipoSimbolo tipo) {
    switch (tipo) {
        case SIMBOLO_VARIAVEL: return "variavel";
        case SIMBOLO_FUNCAO: return "funcao";
        case SIMBOLO_VETOR: return "vetor";
        case SIMBOLO_PARAMETRO: return "parametro";
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

static void imprimirUsos(const ListaLinha *usos) {
    const ListaLinha *atual = usos;
    int primeiro = 1;
    while (atual) {
        if (!primeiro) printf(", ");
        printf("%d", atual->linha);
        primeiro = 0;
        atual = atual->proximo;
    }
}

void imprimirTabelaSimbolos(void) {
    printf("\nTabela de Símbolos:\n");
    printf("Nome\tTipo\tEscopo\tTipo Dado\tDeclarado\tTamanho\tUsos\n");
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        EntradaSimbolo *atual = tabela[i];
        while (atual) {
            printf("%s\t%s\t%s\t%s\t%d\t%d\t",
                   atual->nome,
                   nomeTipoSimbolo(atual->tipoSimbolo),
                   atual->escopo,
                   nomeTipoDado(atual->tipoDado),
                   atual->linhaDeclaracao,
                   atual->tamanho);
            imprimirUsos(atual->usos);
            printf("\n");
            atual = atual->proximo;
        }
    }
}

void limparTabelaSimbolos(void) {
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        EntradaSimbolo *atual = tabela[i];
        while (atual) {
            EntradaSimbolo *tmp = atual->proximo;
            free(atual->nome);
            free(atual->escopo);
            ListaLinha *uso = atual->usos;
            while (uso) {
                ListaLinha *tmpUso = uso->proximo;
                free(uso);
                uso = tmpUso;
            }
            free(atual);
            atual = tmp;
        }
        tabela[i] = NULL;
    }
    while (topoEscopos >= 0) {
        free(pilhaEscopos[topoEscopos--]);
    }
}

const char *escopoAtual(void) {
    if (topoEscopos >= 0) return pilhaEscopos[topoEscopos];
    return "global";
}

void empilharEscopo(const char *nome) {
    if (topoEscopos + 1 >= TAMANHO_MAX_ESCOPOS) return;
    pilhaEscopos[++topoEscopos] = duplicarTexto(nome);
}

void desempilharEscopo(void) {
    if (topoEscopos < 0) return;
    free(pilhaEscopos[topoEscopos]);
    pilhaEscopos[topoEscopos] = NULL;
    topoEscopos--;
}

