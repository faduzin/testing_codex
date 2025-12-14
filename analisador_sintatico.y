%{

#include <stdio.h>
#include <stdlib.h>
extern char *yytext;

extern int contLinha;

static int linhaNo;

void yyerror(const char *mensagem);


int yylex(void);

%}

%union {
  int ival;
  char *sval;
  /* TreeNode *tnode;  (mais tarde, quando AST existir) */
}

%token <sval> ID
%token <ival> NUM
%token IF ELSE INT RETURN VOID WHILE
%token MAIS MENOS MULT BARRA MENOR MENORIGUAL MAIOR MAIORIGUAL IGUAL DIFERENTE RECEBE
%token PONTOVIRGULA VIRGULA ABREPARENTESES FECHAPARENTESES ABRECOLCHETE FECHACOLCHETE ABRECHAVE FECHACHAVE
%token ERROR
%expect 1

%%

programa
	: declaracao-lista
	;
	
declaracao-lista
	: declaracao-lista declaracao
	| declaracao
	;
	
declaracao
	: var-declaracao
	| fun-declaracao
	;
	
var-declaracao
	: tipo-especificador ID PONTOVIRGULA
	| tipo-especificador ID ABRECOLCHETE NUM FECHACOLCHETE PONTOVIRGULA
	;
	
tipo-especificador
	: INT
	| VOID
	;
	
fun-declaracao
	: tipo-especificador ID ABREPARENTESES params FECHAPARENTESES composto-decl
	;
params
	: param-lista
	| VOID
	;
	
param-lista
	: param-lista VIRGULA param
	| param
	;
	
param
	: tipo-especificador ID
	| tipo-especificador ID ABRECOLCHETE FECHACOLCHETE
	;

composto-decl
	: ABRECHAVE local-declaracoes statement-lista FECHACHAVE
	;
	
local-declaracoes
	: local-declaracoes var-declaracao
	|
	;
	
statement-lista
	: statement-lista statement
	|
	;

statement
	: expressao-decl
	| composto-decl
	| selecao-decl
	| iteracao-decl
	| retorno-decl
	;
	
expressao-decl
	: expressao PONTOVIRGULA
	| PONTOVIRGULA
	;
	
selecao-decl
	: IF ABREPARENTESES expressao FECHAPARENTESES statement
	| IF ABREPARENTESES expressao FECHAPARENTESES statement ELSE statement
	;
	
iteracao-decl
	: WHILE ABREPARENTESES expressao FECHAPARENTESES statement
	;
retorno-decl
	: RETURN PONTOVIRGULA
	| RETURN expressao PONTOVIRGULA;
	;
	
expressao
	: var RECEBE expressao
	| simples-expressao
	;
	
var
	: ID
	| ID ABRECOLCHETE expressao FECHACOLCHETE
	;
	
simples-expressao
	: soma-expressao relacional soma-expressao
	| soma-expressao
	;
	
relacional
	: MENORIGUAL
	| MENOR
	| MAIOR
	| MAIORIGUAL
	| IGUAL
	| DIFERENTE
	;

soma-expressao
	: soma-expressao soma termo
	| termo
	;
soma
	: MAIS
	| MENOS
	;
	
termo
	: termo mult fator
	| fator
	;
	
mult
	: MULT
	| BARRA
	;
	
fator
	: ABREPARENTESES expressao FECHAPARENTESES
	| var
	| ativacao
	| NUM
	;
	
ativacao
	: ID ABREPARENTESES args FECHAPARENTESES
	;
	
args
	: arg-lista
	| 
	;
	
arg-lista
	: arg-lista VIRGULA expressao
	| expressao
	;
	
%%

void yyerror(const char *s)
{
    fprintf(stderr,
            "ERRO SINTÁTICO: %s | Linha: %d\n",
            yytext, contLinha);
}
