#include "valor.h"
#include "ast.h"
#include "tabla_hash.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

Valor crear_indefinido() {
    return (Valor) {TIPO_INDEFINIDO, NULL, NULL};
}

Valor crear_nulo() {
    return (Valor) {TIPO_NULO, NULL, NULL};
}

Valor crear_entero(Entero entero, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }
    return (Valor) {TIPO_ENTERO, NULL, loc, .entero = entero};
}

Valor crear_bool(Bool bool, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }
    return (Valor) {TIPO_BOOL, NULL, loc, .entero = bool ? 1 : 0};
}

Valor crear_valor_string(String string, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }
    int *referencias = malloc(sizeof(int));
    *referencias = 1;
    return (Valor) {TIPO_STRING, referencias, loc, .string = string};
}

Valor crear_funcion_nativa(FuncionNativa funcion, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }
    return (Valor) {TIPO_FUNCION_NATIVA, NULL, loc, .funcion_nativa = funcion};
}

Valor crear_funcion(ListaIdentificadores argumentos, struct Expresion *cuerpo, struct TablaHash *capturadas, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }
    int *referencias = malloc(sizeof(int));
    *referencias = 1;

    return (Valor) {
            .tipoValor = TIPO_FUNCION,
            .referencias = referencias,
            .loc = loc,
            .funcion = (Funcion) {
                    .argumentos = argumentos,
                    .cuerpo = cuerpo,
                    .variables_capturadas = capturadas
            }
    };
}

Valor crear_error(Localizacion *loc,const char *formato, ...) {
    String mensaje;
    // Cosas mágicas para pasar un número variable de argumentos.
    va_list args, args2;
    va_start(args, formato);
    va_copy(args2, args);
    // Calcular la longitud total que tendrá el string final.
    size_t longitud = vsnprintf(NULL, 0, formato, args);
    mensaje = crear_string_n(longitud);
    // Escribir el resultado en el string creado.
    char *str = string_a_puntero(&mensaje);
    vsnprintf(str, longitud + 1, formato, args2);
    va_end(args);
    va_end(args2);

    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }

    return (Valor) {
            .tipoValor = TIPO_ERROR,
            .loc = loc,
            .error = mensaje
    };
}

Valor clonar_valor(Valor v) {
    Valor copia = v;
    // Al clonar aumentar el número de referencias si es un
    // valor dinámico (string, error, función, etc).
    if (copia.referencias)
        *copia.referencias += 1;
    return copia;
}

void borrar_valor(Valor *valor) {
    // Si es un valor dinámico (string, error, función, etc),
    // reducir el número de referencias dinámicas; y si estas
    // legan a 0, liberar la memoria.
    if (valor->referencias) {
        *valor->referencias -= 1;
        if (*valor->referencias <= 0) {
            if (valor->loc)
                free(valor->loc);
            switch (valor->tipoValor) {
                case TIPO_ERROR:
                    borrar_string(&valor->error);
                    break;
                case TIPO_STRING:
                    borrar_string(&valor->string);
                    break;
                case TIPO_FUNCION:
                    borrar_lista_identificadores(&valor->funcion.argumentos);
                    borrar_expresion((Expresion *) valor->funcion.cuerpo);
                    free(valor->funcion.cuerpo);
                    borrar_tabla_hash((TablaHash *) valor->funcion.variables_capturadas);
                    free(valor->funcion.variables_capturadas);
                    break;
                default:
                    break;
            }
            free(valor->referencias);
            valor->referencias = NULL;
        }
    }
    valor->tipoValor = TIPO_INDEFINIDO;
}

int comparar_valor(Valor a, Valor b) {
    if (a.tipoValor != b.tipoValor) return 0;
    switch (a.tipoValor) {
        case TIPO_INDEFINIDO:
            return 0;
        case TIPO_NULO:
            return 1;
        case TIPO_ERROR:
            return 1;
        case TIPO_FUNCION_NATIVA:
            return a.funcion_nativa == b.funcion_nativa;
        case TIPO_FUNCION:
            return 0;
        case TIPO_ENTERO:
            return a.entero == b.entero;
        case TIPO_BOOL:
            return a.bool == b.bool;
        case TIPO_STRING:
            return strcmp(string_a_puntero(&a.string), string_a_puntero(&b.string)) == 0;
        default:
            return 0;
    }
}

void borrar_lista_valores(ListaValores *lista) {
    for (int i = 0; i < lista->longitud; ++i)
        borrar_valor(&((Valor *) lista->valores)[i]);
    free(lista->valores);
}

Identificador crear_identificador(String nombre, Localizacion loc) {
    return (Identificador) {
        .nombre = nombre,
        .loc = loc
    };
}

Identificador clonar_identificador(Identificador id) {
    return (Identificador) {
        .nombre = clonar_string(id.nombre),
        .loc = id.loc
    };
}

ListaIdentificadores crear_lista_identificadores() {
    return (ListaIdentificadores) {
            .longitud = 0,
            .capacidad = 0,
            .valores = NULL
    };
}

void push_lista_identificadores(ListaIdentificadores *lista, Identificador identificador) {
    if (lista->longitud >= lista->capacidad) {
        lista->valores = realloc(lista->valores, (lista->capacidad + 1) * sizeof(Identificador));
        ++lista->capacidad;
    }
    ((Identificador *) lista->valores)[lista->longitud++] = identificador;
}

ListaIdentificadores clonar_lista_identificadores(ListaIdentificadores lista) {
    ListaIdentificadores copia = (ListaIdentificadores) {
            .longitud = lista.longitud,
            .capacidad = lista.longitud,
            .valores = malloc(sizeof(String) * lista.longitud)
    };
    for (int i = 0; i < lista.longitud; ++i) {
        copia.valores[i].nombre = clonar_string(lista.valores[i].nombre);
        copia.valores[i].loc = lista.valores[i].loc;
    }
    return copia;
}

void borrar_lista_identificadores(ListaIdentificadores *lista) {
    for (int i = 0; i < lista->longitud; ++i)
        borrar_string(&lista->valores[i].nombre);
    free(lista->valores);
    lista->capacidad = 0;
    lista->longitud = 0;
}

void _imprimir_lista_identificadores(ListaIdentificadores listaIdentificadores) {
    for (int i = 0; i < listaIdentificadores.longitud; ++i) {
        printf("%s, ", string_a_puntero(&listaIdentificadores.valores[i].nombre));
    }
}

void imprimir_lista_identificadores(ListaIdentificadores listaIdentificadores) {
    _imprimir_lista_identificadores(listaIdentificadores);
    printf("\n");
}

void _imprimir_valor(Valor valor) {
    switch (valor.tipoValor) {
        case TIPO_INDEFINIDO: /*printf("indefinido");*/ break;
        case TIPO_NULO:
            printf("null");
            break;
        case TIPO_ERROR:
            if (valor.loc) {
                printf("%d:%d-%d:%d: ",
                       valor.loc->first_line,
                       valor.loc->first_column,
                       valor.loc->last_line,
                       valor.loc->last_column);
            } else {
                printf("Error: ");
            }
            printf("%s", string_a_puntero(&valor.error));
            break;
        case TIPO_FUNCION_NATIVA:
            printf("[función nativa]");
            break;
        case TIPO_FUNCION:
            printf("[función]");
            break;
        case TIPO_ENTERO:
            printf("%d", valor.entero);
            break;
        case TIPO_BOOL:
            printf("%s", valor.bool ? "Verdadero" : "Falso");
            break;
        case TIPO_STRING:
            printf("%s", string_a_puntero(&valor.string));
            break;
    }
}

void imprimir_valor(Valor valor) {
    _imprimir_valor(valor);
    if (valor.tipoValor != TIPO_INDEFINIDO) printf("\n");
}