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
        linea[read-1] = '\0';

        CodigoFuente src = crear_codigo_fuente_str_cpy(linea);
        Lexer lexer = crear_lexer(src);
        Evaluador evaluador = crear_evaluador(lexer, CNTXT_INTERACTIVO, crear_string("."));

        Valor x;
        while (evaluar_siguiente(&evaluador, tabla_simbolos, &x)) {
            if (x.tipo_valor == TIPO_CONTROL_FLUJO && x.control_flujo.tipo == CTR_FLUJO_EXIT) {
                borrar_valor(&x);
                borrar_evaluador(&evaluador);
                free(linea);
                return;
            }

            imprimir_valor(x);
            borrar_valor(&x);
        }
        borrar_evaluador(&evaluador);

        printf("> ");
    }
    free(linea);
}

void modo_fichero(TablaSimbolos *simbolos, char* fichero) {
    CodigoFuente src;
    if (!crear_codigo_fuente_archivo(fichero, &src)) {
        printf("No se pudo abrir el archivo \"%s\".", fichero);
        return;
    }

    Lexer lexer = crear_lexer(src);
    Evaluador evaluador = crear_evaluador(lexer, CNTXT_INTERACTIVO, crear_string("."));

    Valor x;
    while(evaluar_siguiente(&evaluador, simbolos, &x)) {
        if (x.tipo_valor == TIPO_CONTROL_FLUJO && x.control_flujo.tipo == CTR_FLUJO_EXIT) {
            borrar_valor(&x);
            borrar_evaluador(&evaluador);
            return;
        }

        imprimir_valor(x);
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
        if (argc > 2) printf("Demasiados argumentos; ignorando.\n");
        modo_fichero(&simbolos, argv[1]);
    }

    borrar_tabla_simbolos(&simbolos);
    borrar_buffer_strings();

    return 0;
}