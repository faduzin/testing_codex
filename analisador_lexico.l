%{

#include <stdio.h>
#include <stdlib.h>
#include "cmenos.tab.h"

int contLinha = 1;

void erroLexico(char *mensagem)
{
	printf("ERRO LEXICO: %s LINHA: %d\n", mensagem, contLinha);
	exit(1);
}

%}

DIGITO [0-9]
NUMERO {DIGITO}+
LETRA [a-zA-Z]
ID {LETRA}+
NOVALINHA \n
ESPACO [ \t]+

%%
"if"            { return IF; }
"else" { return ELSE; }
"int" { return INT; }
"return"        { return RETURN; }
"void"          { return VOID; }
"while"         { return WHILE; }

"+"             { return MAIS; }
"-"             { return MENOS; }
"*"             { return MULT; }
"/"             { return BARRA; }
"<"             { return MENOR; }
"<="            { return MENORIGUAL; }
">"             { return MAIOR; }
">="            { return MAIORIGUAL; }
"=="            { return IGUAL; }
"!="            { return DIFERENTE; }
"="             { return RECEBE; }
";"             { return PONTOVIRGULA; }
","             { return VIRGULA; }
"("             { return ABREPARENTESES; }
")"             { return FECHAPARENTESES; }
"["             { return ABRECOLCHETE; }
"]"             { return FECHACOLCHETE; }
"{"             { return ABRECHAVE; }
"}"             { return FECHACHAVE; }

{NUMERO}  { 
    yylval.ival = atoi(yytext);   /* <-- passa o valor numérico */
    return NUM;
}

{ID}  {
    yylval.sval = strdup(yytext); /* <-- passa o lexema do identificador */
    return ID;
}

{NOVALINHA}       { contLinha++; }
{ESPACO}    { }

"/*" {
    char c;
    while(1) {
        c = input();
        if (c == EOF) break; /* Segurança contra fim de arquivo inesperado */
        
        if (c == '\n') {
            contLinha++; /* Importante: manter a contagem de linhas atualizada */
        }
        
        if (c == '*') {
            char next = input();
            if (next == '/') break; /* Encontrou fim do comentário */
            if (next == EOF) break;
            
            /* Se não for barra, devolve o caractere para o buffer para ser reprocessado */
            if (next != '*') unput(next); 
            /* Nota: se for **, precisamos ter cuidado, mas o loop trata na próxima iteração */
        }
    }
}

"//"[^\n]*   { /* ignora comentário de linha */ }
.            { erroLexico(yytext); }

%%
	int yywrap(){ return 1;}
