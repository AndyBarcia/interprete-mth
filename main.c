#include <stdio.h>

#include "tabla_hash.h"
#include "string.h"
#include "std.h"
#include "evaluador.h"

void modo_interactivo(TablaSimbolos *tabla_simbolos) {
    char* linea = NULL;
    size_t len = 0;
    ssize_t read;

    printf("> ");
    while ((read = getline(&linea, &len, stdin)) != -1) {
        Lexer lexer = crear_lexer_str(linea);
        Evaluador evaluador = crear_evaluador(lexer);

        Valor x;
        if(evaluar_siguiente(&evaluador, tabla_simbolos, &x)) {
            if (x.tipo_valor == TIPO_ERROR) {
                imprimir_error(x.error, NULL, linea, x.loc);
            } else {
                imprimir_valor(x);
            }
            borrar_valor(&x);
        }
        borrar_evaluador(&evaluador);

        printf("> ");
    }
    free(linea);
}

void modo_fichero(TablaSimbolos *simbolos, char* fichero) {
    Lexer lexer;
    if (!crear_lexer_archivo(&lexer, fichero)) {
        printf("No se pudo abrir el archivo \"%s\".", fichero);
        return;
    }

    Evaluador evaluador = crear_evaluador(lexer);

    Valor x;
    while(evaluar_siguiente(&evaluador, simbolos, &x)) {
        if (x.tipo_valor == TIPO_ERROR) {
            char* linea = obtener_linea(lexer, x.loc->first_line);
            imprimir_error(x.error, fichero, linea, x.loc);
        }
        borrar_valor(&x);
    }

    borrar_evaluador(&evaluador);
}

int main(int argc, char *argv[]) {
    crear_buffer_strings();

    TablaSimbolos simbolos = crear_tabla_simbolos();
    inicializar_libreria_estandar(&simbolos);

    if (argc <= 1) {
        modo_interactivo(&simbolos);
    }else {
        if (argc > 2) printf("Demasiados nombres_args; ignorando.\n");
        modo_fichero(&simbolos, argv[1]);
    }

    borrar_tabla_simbolos(&simbolos);
    borrar_buffer_strings();

    return 0;
}