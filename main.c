#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "semantico.h"

extern FILE *yyin;
int yyparse(void);

extern ASTNo *arvoreSintatica;
extern int errosLexicos;
extern int errosSintaticos;
extern int errosSemanticos;

int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("Erro ao abrir arquivo");
            return 1;
        }
    }

    iniciarAnaliseSemantica();
    yyparse();

    printf("\nAST Gerada:\n");
    if (arvoreSintatica) {
        imprimirAST(arvoreSintatica, 0);
    } else {
        printf("(AST vazia devido a erros de sintaxe)\n");
    }

    imprimirTabelaSimbolos();

    if (errosLexicos + errosSintaticos + errosSemanticos > 0) {
        printf("\nResumo de Erros -> Léxicos: %d, Sintáticos: %d, Semânticos: %d\n",
               errosLexicos, errosSintaticos, errosSemanticos);
    } else {
        printf("\nCompilação concluída sem erros.\n");
    }

    limparTabelaSimbolos();
    return (errosLexicos + errosSintaticos + errosSemanticos) ? EXIT_FAILURE : EXIT_SUCCESS;
}

