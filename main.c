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

        Lexer lexer = crear_lexer_str(linea);
        Evaluador evaluador = crear_evaluador(lexer, CNTXT_INTERACTIVO, crear_string("."));

        Valor x;
        while (evaluar_siguiente(&evaluador, tabla_simbolos, &x)) {
            switch (x.tipo_valor) {
                case TIPO_ERROR: {
                    imprimir_error(x.error, NULL, linea, x.loc);
                    break;
                }
                case TIPO_CONTROL_FLUJO: {
                    borrar_valor(&x);
                    borrar_evaluador(&evaluador);
                    free(linea);
                    return;
                }
                default: imprimir_valor(x);
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

    Evaluador evaluador = crear_evaluador(lexer, CNTXT_INTERACTIVO, crear_string("."));

    Valor x;
    while(evaluar_siguiente(&evaluador, simbolos, &x)) {
        switch (x.tipo_valor) {
            case TIPO_ERROR: {
                char* linea = obtener_linea(lexer, x.loc->first_line);
                imprimir_error(x.error, NULL, linea, x.loc);
                break;
            }
            case TIPO_CONTROL_FLUJO: {
                borrar_valor(&x);
                borrar_evaluador(&evaluador);
                return;
            }
            default: break;
        }
        borrar_valor(&x);
    }

    borrar_evaluador(&evaluador);
}

#include "bibliotecas/ffi.h"
#include <dlfcn.h>

int main(int argc, char *argv[]) {

    /*void* libhandle = dlopen("../tests/test.so", RTLD_LAZY);
    if (!libhandle) {
        fprintf(stderr, "dlopen error: %s\n", dlerror());
        exit(1);
    }

    printf("dlopen success: handle %p\n", libhandle);

    void* add_data_fn = dlsym(libhandle, "sumar");
    char* err = dlerror();
    if (err) {
        fprintf(stderr, "dlsym failed: %s\n", err);
        exit(1);
    }

    ffi_type* arg_types[] = {&ffi_type_sint, &ffi_type_sint};
    ffi_type* rtype = &ffi_type_sint;
    ffi_cif cif;
    ffi_status status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 2, rtype, arg_types);
    if (status != FFI_OK) {
        fprintf(stderr, "ffi_prep_cif failed: %d\n", status);
        exit(1);
    }

    int arg1 = 2;
    int arg2 = 3;

    void *args[2] = {&arg1,&arg2};
    int out;
    ffi_call(&cif, FFI_FN(add_data_fn), &out, args);

    printf("%d\n", out);

    return 0;*/

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