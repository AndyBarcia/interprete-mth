#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "tabla_hash.h"
#include "string.h"
#include "std.h"
#include "evaluador.h"

void modo_interactivo(Evaluador *evaluador) {
    char* linea = NULL;
    size_t len = 0;
    ssize_t read;

    printf("> ");
    while ((read = getline(&linea, &len, stdin)) != -1) {
        linea[read-1] = '\0';

        CodigoFuente src = crear_codigo_fuente_str_cpy(linea);
        Lexer lexer = crear_lexer(src);
        Parser parser = crear_parser(lexer, CONTEXTO_INTERACTIVO);

        Expresion exp;
        while (siguiente_expresion(&parser, &exp)) {
            Valor v = evaluar_expresion(evaluador, &exp);
            if (v.tipo_valor == TIPO_CONTROL_FLUJO && v.control_flujo.tipo == CTR_FLUJO_EXIT) {
                borrar_expresion(&exp);
                borrar_valor(&v);
                borrar_parser(&parser);
                free(linea);
                return;
            }

            imprimir_valor(v);
            borrar_valor(&v);
            borrar_expresion(&exp);
        }
        borrar_parser(&parser);

        printf("> ");
    }
    free(linea);
}

void modo_fichero(Evaluador *evaluador, char* fichero) {
    // Trata de cargar el código fuente
    CodigoFuente src;
    if (!crear_codigo_fuente_archivo(fichero, &src)) {
        printf("No se pudo abrir el archivo \"%s\".\n", fichero);
        return;
    }

    // Obtiene el directorio de trabajo en base al fichero.
    // y cambia el directorio de trabajo del evaluador.
    char* subdir = fichero;
    long longitud_subdir = strlen(subdir);
    for (long i = longitud_subdir; i > 0; --i) {
        if (subdir[i] == '/') {
            extender_string(&evaluador->wd, "/");
            extender_string_n(&evaluador->wd, subdir, i);
            break;
        }
    }

    Lexer lexer = crear_lexer(src);
    Parser parser = crear_parser(lexer, CONTEXTO_INTERACTIVO);

    Expresion exp;
    while (siguiente_expresion(&parser, &exp)) {
        Valor v = evaluar_expresion(evaluador, &exp);
        if (v.tipo_valor == TIPO_CONTROL_FLUJO && v.control_flujo.tipo == CTR_FLUJO_EXIT) {
            borrar_expresion(&exp);
            borrar_valor(&v);
            borrar_parser(&parser);
            return;
        }

        imprimir_valor(v);
        borrar_valor(&v);
        borrar_expresion(&exp);
    }

    borrar_parser(&parser);
}

#define DEFAULT_PRELUDE_FILE "prelude.mth"

int main(int argc, char *argv[]) {
    // Variables para las opciones
    int no_prelude = 0;
    char *prelude_file = NULL;
    int debug = 0;
    int help = 0;

    // Opciones permitidas
    struct option long_options[] = {
            {"no-prelude", no_argument, &no_prelude, 1},
            {"prelude", required_argument, NULL, 'p'},
            {"debug", no_argument, &debug, 1},
            {"help", no_argument, &help, 1},
            {NULL, 0, NULL, 0}
    };

    // Deshabilitar la impresión de errores por defecto de getopt_long()
    opterr = 0;

    // Procesamiento de opciones
    int c, option_index = 0;
    while ((c = getopt_long(argc, argv, "p:dh", long_options, &option_index)) != -1) {
        switch (c) {
            case 0:
                break;
            case 'p':
                prelude_file = optarg;
                break;
            case 'd':
                debug = 1;
                break;
            case 'h':
                help = 1;
                break;
            case '?':
                printf("error: opción '%s' no reconocida. Ignorando\n", argv[option_index+1]);
                break;
            default:
                abort();
        }
    }

    // Mostrar el mensaje de ayuda si se estableció como opción.
    if (help) {
        printf("Intérprete de un lenguaje básico matemático. Si no se especifica un fichero"
               " a ejecutar, se inicia el modo interactivo.\n"
               "Por defecto se carga un módulo con definiciones básicas de la biblioteca estándar"
               " (senos, cosenos, constantes matemáticas, etc). Si se quiere no cargar un"
               " módulo por defecto, o se quiere cargar otro, se pueden utilizar las opciones.\n"
               "\n"
               "math [OPCIONES] [fichero] \n"
               "\nOPCIONES:\n"
               "\t-p --prelude fichero Este es el módulo que se cargará por defecto (\"prelude.mth\" por defecto)\n"
               "\t   --no-prelude No importa ningún módulo por defecto\n"
               "\t-d --debug Imprime las expresiones antes de evaluarlas\n"
               "\t-h --help Imprime este mensaje de ayuda y termina\n"
               "\n"
               "Para más información utiliza en modo interactivo la función `help()`.\n"
               "Para salir del modo interactivo, utiliza la función `exit()`.\n");
        return 0;
    }

    // Comprobar que la combinación de opciones pasadas es válida.
    if (prelude_file && no_prelude) {
        printf("aviso: se ha indicado un módulo como prelude, pero se ha establecido que no se "
               "cargará un prelude. Ignorando prelude.\n");
    } else if (!prelude_file) {
        prelude_file = DEFAULT_PRELUDE_FILE;
    }

    // Comprobar si se han pasado argumentos posicionales adicionales.
    int n_argumentos_posicionales = argc - optind;
    char** argumentos_posicionales = argv + optind;
    if (n_argumentos_posicionales > 1) {
        printf("error: se han pasado demasiados argumentos posicionales. Ignorando.\n");
    }

    // Crear la tabla de símbolos básica con las intrínsecas.
    TablaSimbolos simbolos = crear_tabla_simbolos();
    cargar_intrinsecas(&simbolos);

    // Crea un evaluador en el directorio actual
    Evaluador evaluador = crear_evaluador(&simbolos, crear_string("."));

    // Carga el módulo prelude en caso de que sea necesario.
    if (!no_prelude) {
        Expresion e = crear_exp_importe(crear_string(prelude_file), 0, NULL);
        Valor v = evaluar_expresion(&evaluador, &e);
        if (v.tipo_valor == TIPO_ERROR) imprimir_valor(v);
        borrar_expresion(&e);
    }

    // Establecer el modo debug si se estableció como argumento.
    // No lo establecemos antes para que no se pete la terminal al
    // cargarse el prelude.
    evaluador.debug = debug;

    if (n_argumentos_posicionales < 1) {
        modo_interactivo(&evaluador);
    }else {
        modo_fichero(&evaluador, argumentos_posicionales[0]);
    }

    borrar_evaluador(&evaluador);
    borrar_tabla_simbolos(&simbolos);

    return 0;
}