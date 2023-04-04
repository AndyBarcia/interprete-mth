#include <stdio.h>

#include "tabla_hash.h"
#include "string.h"
#include "std.h"
#include "evaluador.h"

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

    TablaSimbolos simbolos = crear_tabla_simbolos();
    inicializar_libreria_estandar(&simbolos);

    Valor x = evaluar_fichero(&simbolos, entrada);
    borrar_valor(&x);

    borrar_tabla_simbolos(&simbolos);
    borrar_buffer_strings();

    if (entrada != stdin)
        fclose(entrada);

    return 0;
}