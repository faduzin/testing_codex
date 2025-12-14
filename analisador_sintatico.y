%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantico.h"

extern char *yytext;
extern int contLinha;
extern int errosLexicos;

static char *duplicarLexema(const char *texto) {
    if (!texto) return NULL;
    size_t tam = strlen(texto) + 1;
    char *copia = (char *)malloc(tam);
    if (!copia) return NULL;
    memcpy(copia, texto, tam);
    return copia;
}

ASTNo *arvoreSintatica = NULL;
int errosSintaticos = 0;

void yyerror(const char *mensagem);
int yylex(void);
%}

%code requires {
#include "ast.h"
#include "semantico.h"
}

%output "cmenos.tab.c"
%defines "cmenos.tab.h"
%define parse.error verbose

%union {
  int ival;
  char *sval;
  ASTNo *tnode;
  TipoDado tipo;
}

%token <sval> ID
%token <ival> NUM
%token IF ELSE INT RETURN VOID WHILE
%token MAIS MENOS MULT BARRA MENOR MENORIGUAL MAIOR MAIORIGUAL IGUAL DIFERENTE RECEBE
%token PONTOVIRGULA VIRGULA ABREPARENTESES FECHAPARENTESES ABRECOLCHETE FECHACOLCHETE ABRECHAVE FECHACHAVE
%token ERROR
%expect 1

%type <tnode> programa declaracao_lista declaracao var_declaracao fun_declaracao params param_lista param local_declaracoes DeclaracaoErro
%type <tnode> statement_lista statement expressao_decl selecao_decl iteracao_decl retorno_decl expressao var simples_expressao
%type <tnode> soma_expressao termo fator ativacao args arg_lista bloco_escopo
%type <sval> relacional soma mult
%type <tipo> tipo_especificador

%%

programa
        : declaracao_lista { arvoreSintatica = $1; }
        ;

DeclaracaoErro
        : error PONTOVIRGULA { errosSintaticos++; yyerrok; $$ = NULL; }
        ;

declaracao_lista
        : declaracao_lista declaracao { if ($1 && $2) anexarIrmao($1, $2); $$ = $1 ? $1 : $2; }
        | declaracao { $$ = $1; }
        ;

declaracao
        : var_declaracao
        | fun_declaracao
        ;

var_declaracao
        : tipo_especificador ID PONTOVIRGULA {
              $$ = criarNoAST(NO_DECL_VAR, $2, 0, contLinha);
              definirTipoNo($$, $1);
              registrarDeclaracaoIdentificador($2, SIMBOLO_VARIAVEL, $1, contLinha, 1);
          }
        | tipo_especificador ID ABRECOLCHETE NUM FECHACOLCHETE PONTOVIRGULA {
              $$ = criarNoAST(NO_VETOR, $2, $4, contLinha);
              definirTipoNo($$, $1);
              registrarDeclaracaoIdentificador($2, SIMBOLO_VETOR, $1, contLinha, $4);
          }
        ;


tipo_especificador
        : INT { $$ = TIPO_DADO_INT; }
        | VOID { $$ = TIPO_DADO_VOID; }
        ;

fun_declaracao
        : tipo_especificador ID { registrarDeclaracaoIdentificador($2, SIMBOLO_FUNCAO, $1, contLinha, 0); entrarEscopoFuncao($2); }
          ABREPARENTESES params FECHAPARENTESES bloco_escopo {
              $$ = criarNoAST(NO_DECL_FUN, $2, 0, contLinha);
              definirTipoNo($$, $1);
              adicionarFilho($$, $5, 0);
              adicionarFilho($$, $7, 1);
              sairEscopoAtual();
          }
        ;

params
        : param_lista { $$ = $1; }
        | VOID { $$ = NULL; }
        ;

param_lista
        : param_lista VIRGULA param { if ($1 && $3) anexarIrmao($1, $3); $$ = $1 ? $1 : $3; }
        | param { $$ = $1; }
        ;

param
        : tipo_especificador ID {
              registrarDeclaracaoIdentificador($2, SIMBOLO_PARAMETRO, $1, contLinha, 1);
              $$ = criarNoAST(NO_PARAM, $2, 0, contLinha);
              definirTipoNo($$, $1);
          }
        | tipo_especificador ID ABRECOLCHETE FECHACOLCHETE {
              registrarDeclaracaoIdentificador($2, SIMBOLO_PARAMETRO, $1, contLinha, 0);
              $$ = criarNoAST(NO_PARAM, $2, 0, contLinha);
              definirTipoNo($$, $1);
          }
        ;

bloco_escopo
        : ABRECHAVE { entrarEscopoBloco(); }
          local_declaracoes statement_lista FECHACHAVE {
              $$ = criarNoAST(NO_BLOCO, NULL, 0, contLinha);
              adicionarFilho($$, $3, 0);
              adicionarFilho($$, $4, 1);
              sairEscopoAtual();
          }
        ;

local_declaracoes
        : local_declaracoes var_declaracao { if ($1 && $2) anexarIrmao($1, $2); $$ = $1 ? $1 : $2; }
        | /* vazio */ { $$ = NULL; }
        ;

statement_lista
        : statement_lista statement { if ($1 && $2) anexarIrmao($1, $2); $$ = $1 ? $1 : $2; }
        | /* vazio */ { $$ = NULL; }
        ;

statement
        : expressao_decl
        | bloco_escopo
        | selecao_decl
        | iteracao_decl
        | retorno_decl
        | DeclaracaoErro
        ;

expressao_decl
        : expressao PONTOVIRGULA { $$ = $1; }
        | PONTOVIRGULA { $$ = NULL; }
        ;

selecao_decl
        : IF ABREPARENTESES expressao FECHAPARENTESES statement {
              $$ = criarNoAST(NO_SELECAO, NULL, 0, contLinha);
              adicionarFilho($$, $3, 0);
              adicionarFilho($$, $5, 1);
          }
        | IF ABREPARENTESES expressao FECHAPARENTESES statement ELSE statement {
              $$ = criarNoAST(NO_SELECAO, NULL, 0, contLinha);
              adicionarFilho($$, $3, 0);
              adicionarFilho($$, $5, 1);
              adicionarFilho($$, $7, 2);
          }
        ;

iteracao_decl
        : WHILE ABREPARENTESES expressao FECHAPARENTESES statement {
              $$ = criarNoAST(NO_ITERACAO, NULL, 0, contLinha);
              adicionarFilho($$, $3, 0);
              adicionarFilho($$, $5, 1);
          }
        ;

retorno_decl
        : RETURN PONTOVIRGULA { $$ = criarNoAST(NO_RETORNO, "return", 0, contLinha); }
        | RETURN expressao PONTOVIRGULA {
              $$ = criarNoAST(NO_RETORNO, "return", 0, contLinha);
              adicionarFilho($$, $2, 0);
          }
        ;

expressao
        : var RECEBE expressao {
              $$ = criarNoAST(NO_ATRIBUICAO, "=", 0, contLinha);
              adicionarFilho($$, $1, 0);
              adicionarFilho($$, $3, 1);
          }
        | simples_expressao { $$ = $1; }
        ;

var
        : ID {
              registrarUsoIdentificador($1, contLinha);
              $$ = criarNoAST(NO_IDENTIFICADOR, $1, 0, contLinha);
          }
        | ID ABRECOLCHETE expressao FECHACOLCHETE {
              registrarUsoIdentificador($1, contLinha);
              $$ = criarNoAST(NO_VETOR, $1, 0, contLinha);
              adicionarFilho($$, $3, 0);
          }
        ;

simples_expressao
        : soma_expressao relacional soma_expressao {
              $$ = criarNoAST(NO_OPERACAO_REL, $2, 0, contLinha);
              adicionarFilho($$, $1, 0);
              adicionarFilho($$, $3, 1);
          }
        | soma_expressao { $$ = $1; }
        ;

relacional
        : MENORIGUAL { $$ = duplicarLexema("<="); }
        | MENOR { $$ = duplicarLexema("<"); }
        | MAIOR { $$ = duplicarLexema(">"); }
        | MAIORIGUAL { $$ = duplicarLexema(">="); }
        | IGUAL { $$ = duplicarLexema("=="); }
        | DIFERENTE { $$ = duplicarLexema("!="); }
        ;

soma_expressao
        : soma_expressao soma termo {
              $$ = criarNoAST(NO_OPERACAO_SOMA, $2, 0, contLinha);
              adicionarFilho($$, $1, 0);
              adicionarFilho($$, $3, 1);
          }
        | termo { $$ = $1; }
        ;

soma
        : MAIS { $$ = duplicarLexema("+"); }
        | MENOS { $$ = duplicarLexema("-"); }
        ;

termo
        : termo mult fator {
              $$ = criarNoAST(NO_OPERACAO_MULT, $2, 0, contLinha);
              adicionarFilho($$, $1, 0);
              adicionarFilho($$, $3, 1);
          }
        | fator { $$ = $1; }
        ;

mult
        : MULT { $$ = duplicarLexema("*"); }
        | BARRA { $$ = duplicarLexema("/"); }
        ;

fator
        : ABREPARENTESES expressao FECHAPARENTESES { $$ = $2; }
        | var { $$ = $1; }
        | ativacao { $$ = $1; }
        | NUM { $$ = criarNoAST(NO_NUMERO, NULL, $1, contLinha); }
        ;

ativacao
        : ID ABREPARENTESES args FECHAPARENTESES {
              registrarUsoIdentificador($1, contLinha);
              $$ = criarNoAST(NO_ATIVACAO, $1, 0, contLinha);
              adicionarFilho($$, $3, 0);
          }
        ;

args
        : arg_lista { $$ = $1; }
        | /* vazio */ { $$ = NULL; }
        ;

arg_lista
        : arg_lista VIRGULA expressao { if ($1 && $3) anexarIrmao($1, $3); $$ = $1 ? $1 : $3; }
        | expressao { $$ = $1; }
        ;

%%

void yyerror(const char *s)
{
    (void)s;
    fprintf(stderr, "ERRO SINTÁTICO: %s LINHA: %d\n", yytext, contLinha);
    errosSintaticos++;
}
