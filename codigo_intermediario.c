#include "codigo_intermediario.h"
#include <stdlib.h>
#include <string.h>

static int contadorTemp = 0;
static int contadorLabel = 0;

static char *duplicarTexto(const char *texto) {
    if (!texto) return NULL;
    size_t tamanho = strlen(texto) + 1;
    char *copia = (char *)malloc(tamanho);
    if (!copia) return NULL;
    memcpy(copia, texto, tamanho);
    return copia;
}

static char *novoTemp(void) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "t%d", contadorTemp++);
    return duplicarTexto(buffer);
}

static char *novoLabel(void) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "L%d", contadorLabel++);
    return duplicarTexto(buffer);
}

static void gerarListaStatements(ASTNo *no, FILE *destino);
static char *gerarExpressao(ASTNo *no, FILE *destino);
static void gerarStatement(ASTNo *no, FILE *destino);

static char *gerarVarOuVetor(ASTNo *no, FILE *destino) {
    if (!no) return NULL;
    if (no->tipo == NO_IDENTIFICADOR) {
        return duplicarTexto(no->lexema);
    }
    if (no->tipo == NO_VETOR) {
        char *indice = NULL;
        if (no->filhos[0]) {
            indice = gerarExpressao(no->filhos[0], destino);
        }
        size_t tam = strlen(no->lexema) + 3 + (indice ? strlen(indice) : 0);
        char *resultado = (char *)malloc(tam);
        if (resultado) {
            snprintf(resultado, tam, "%s[%s]", no->lexema, indice ? indice : "0");
        }
        free(indice);
        return resultado;
    }
    return NULL;
}

static char *gerarExpressao(ASTNo *no, FILE *destino) {
    if (!no) return NULL;
    switch (no->tipo) {
        case NO_NUMERO: {
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%d", no->valor);
            return duplicarTexto(buffer);
        }
        case NO_IDENTIFICADOR:
            return duplicarTexto(no->lexema);
        case NO_VETOR: {
            return gerarVarOuVetor(no, destino);
        }
        case NO_ATIVACAO: {
            ASTNo *arg = no->filhos[0];
            while (arg) {
                char *valorArg = gerarExpressao(arg, destino);
                if (valorArg) {
                    fprintf(destino, "param %s\n", valorArg);
                    free(valorArg);
                }
                arg = arg->irmao;
            }
            char *temp = novoTemp();
            fprintf(destino, "%s = call %s\n", temp, no->lexema ? no->lexema : "funcao");
            return temp;
        }
        case NO_OPERACAO_SOMA:
        case NO_OPERACAO_MULT: {
            const char *op = no->lexema ? no->lexema : (no->tipo == NO_OPERACAO_SOMA ? "+" : "*");
            char *esq = gerarExpressao(no->filhos[0], destino);
            char *dir = gerarExpressao(no->filhos[1], destino);
            char *temp = novoTemp();
            fprintf(destino, "%s = %s %s %s\n", temp, esq ? esq : "0", op, dir ? dir : "0");
            free(esq);
            free(dir);
            return temp;
        }
        case NO_OPERACAO_REL: {
            const char *op = no->lexema ? no->lexema : "==";
            char *esq = gerarExpressao(no->filhos[0], destino);
            char *dir = gerarExpressao(no->filhos[1], destino);
            char *temp = novoTemp();
            fprintf(destino, "%s = %s %s %s\n", temp, esq ? esq : "0", op, dir ? dir : "0");
            free(esq);
            free(dir);
            return temp;
        }
        default:
            return NULL;
    }
}

static void gerarStatement(ASTNo *no, FILE *destino) {
    if (!no) return;
    switch (no->tipo) {
        case NO_ATRIBUICAO: {
            char *destinoVar = gerarVarOuVetor(no->filhos[0], destino);
            char *valor = gerarExpressao(no->filhos[1], destino);
            if (destinoVar && valor) {
                fprintf(destino, "%s = %s\n", destinoVar, valor);
            }
            free(destinoVar);
            free(valor);
            break;
        }
        case NO_SELECAO: {
            char *labelElse = novoLabel();
            char *labelFim = no->filhos[2] ? novoLabel() : NULL;
            char *cond = gerarExpressao(no->filhos[0], destino);
            fprintf(destino, "ifFalse %s goto %s\n", cond ? cond : "0", labelElse);
            free(cond);
            gerarStatement(no->filhos[1], destino);
            if (no->filhos[2]) {
                fprintf(destino, "goto %s\n", labelFim);
            }
            fprintf(destino, "%s:\n", labelElse);
            if (no->filhos[2]) {
                gerarStatement(no->filhos[2], destino);
                fprintf(destino, "%s:\n", labelFim);
            }
            free(labelElse);
            free(labelFim);
            break;
        }
        case NO_ITERACAO: {
            char *labelInicio = novoLabel();
            char *labelSaida = novoLabel();
            fprintf(destino, "%s:\n", labelInicio);
            char *cond = gerarExpressao(no->filhos[0], destino);
            fprintf(destino, "ifFalse %s goto %s\n", cond ? cond : "0", labelSaida);
            free(cond);
            gerarStatement(no->filhos[1], destino);
            fprintf(destino, "goto %s\n", labelInicio);
            fprintf(destino, "%s:\n", labelSaida);
            free(labelInicio);
            free(labelSaida);
            break;
        }
        case NO_RETORNO: {
            if (no->filhos[0]) {
                char *valor = gerarExpressao(no->filhos[0], destino);
                fprintf(destino, "return %s\n", valor ? valor : "0");
                free(valor);
            } else {
                fprintf(destino, "return\n");
            }
            break;
        }
        case NO_BLOCO: {
            gerarListaStatements(no->filhos[1], destino);
            break;
        }
        case NO_DECL_FUN: {
            fprintf(destino, "func %s:\n", no->lexema ? no->lexema : "anon");
            gerarStatement(no->filhos[1], destino);
            fprintf(destino, "endfunc\n");
            break;
        }
        default: {
            char *resultado = gerarExpressao(no, destino);
            free(resultado);
            break;
        }
    }
}

static void gerarListaStatements(ASTNo *no, FILE *destino) {
    ASTNo *atual = no;
    while (atual) {
        gerarStatement(atual, destino);
        atual = atual->irmao;
    }
}

void gerarCodigoIntermediario(ASTNo *raiz, FILE *destino) {
    if (!raiz || !destino) return;
    gerarListaStatements(raiz, destino);
}
