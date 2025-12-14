#include <stdio.h>

extern FILE *yyin;
int yyparse(void);


int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("Erro ao abrir arquivo");
            return 1;
        }
    }

    yyparse();
    imprimirArvore(arvoreSintatica);
    return 0;
}

