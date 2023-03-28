#include <stdio.h>

#include "analizador_lexico.h"
#include "analizador_sintactico.h"
#include "tabla_hash.h"
#include "string.h"
#include "std.h"

int main(int argc, char *argv[]) {

    FILE* entrada;
    if (argc <= 1) {
        entrada = stdin;
    }else {
        if (argc > 2) printf("Demasiados argumentos; ignorando.\n");

        char *fichero = argv[1];
        printf("Abriendo \"%s\"\n", fichero);
        if ((entrada = fopen(fichero, "r")) == NULL) {
            printf("No se pudo abrir el archivo \"%s\"\n", fichero);
            exit(1);
        }
    }

    crear_buffer_strings();
    establecer_fichero_entrada(entrada);
    inicializar_analizador_lexico();

    TablaSimbolos tablaSimbolos = crear_tabla_simbolos();
    inicializar_libreria_estandar(&tablaSimbolos);

    printf("> ");
    fflush(stdout);

    yyparse(tablaSimbolos);

    borrar_analizador_lexico();
    borrar_buffer_strings();
    return 0;
}