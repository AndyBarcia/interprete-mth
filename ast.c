#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

Valor crear_indefinido() {
    return (Valor) {
            .tipoValor = TIPO_INDEFINIDO,
    };
}

Valor crear_nulo() {
    return (Valor) {
            .tipoValor = TIPO_NULO,
    };
}

Valor crear_entero(Entero entero) {
    return (Valor) {
        .tipoValor = TIPO_ENTERO,
        .entero = entero
    };
}

Valor crear_bool(Bool bool) {
    return (Valor) {
            .tipoValor = TIPO_BOOL,
            .entero = bool ? 1 : 0
    };
}

Valor crear_funcion_nativa(FuncionNativa funcion) {
    return (Valor) {
        .tipoValor = TIPO_FUNCION_NATIVA,
        .funcion_nativa = funcion
    };
}

Valor crear_funcion(ListaIdentificadores argumentos, Expresion cuerpo) {
    Expresion* e = malloc(sizeof (Expresion));
    *e = cuerpo;

    return (Valor) {
        .tipoValor = TIPO_FUNCION,
        .funcion = (Funcion) {
            .argumentos = argumentos,
            .cuerpo = (struct Expresion*) e
        }
    };
}

Valor crear_error(const char *formato, ...) {
    String error;
    // Cosas mágicas para pasar un número variable de argumentos.
    va_list args, args2;
    va_start(args, formato);
    va_copy(args2, args);
    // Calcular la longitud total que tendrá el string final.
    size_t longitud = vsnprintf(NULL, 0, formato, args);
    error = crear_string_n(longitud);
    // Escribir el resultado en el string creado.
    char *str = string_a_puntero(&error);
    vsnprintf(str, longitud + 1, formato, args2);
    va_end(args);
    va_end(args2);

    return (Valor) {
            .tipoValor = TIPO_ERROR,
            .error = error
    };
}

void _imprimir_valor(Valor valor) {
    switch (valor.tipoValor) {
        case TIPO_INDEFINIDO: /*printf("indefinido");*/ break;
        case TIPO_NULO: printf("null"); break;
        case TIPO_ERROR: printf("Error: %s", string_a_puntero(&valor.error)); break;
        case TIPO_FUNCION_NATIVA: printf("[función nativa]"); break;
        case TIPO_FUNCION: printf("[función]"); break;
        case TIPO_ENTERO: printf("%d", valor.entero); break;
        case TIPO_BOOL: printf("%s", valor.bool ? "Verdadero" : "Falso"); break;
    }
}

void imprimir_valor(Valor valor) {
    _imprimir_valor(valor);
    if (valor.tipoValor != TIPO_INDEFINIDO) printf("\n");
}

int comparar_valor(Valor a, Valor b) {
    if (a.tipoValor != b.tipoValor) return 0;
    switch (a.tipoValor) {
        case TIPO_INDEFINIDO: return 0;
        case TIPO_NULO: return 1;
        case TIPO_ERROR: return 1;
        case TIPO_FUNCION_NATIVA: return a.funcion_nativa == b.funcion_nativa;
        case TIPO_FUNCION: return 0;
        case TIPO_ENTERO: return a.entero == b.entero;
        case TIPO_BOOL: return a.bool == b.bool;
    }
}

void _imprimir_lista_identificadores(ListaIdentificadores listaIdentificadores) {
    for (int i = 0; i < listaIdentificadores.longitud; ++i) {
        printf("%s, ", string_a_puntero(&listaIdentificadores.valores[i]));
    }
}

void imprimir_lista_identificadores(ListaIdentificadores listaIdentificadores) {
    _imprimir_lista_identificadores(listaIdentificadores);
    printf("\n");
}

void _imprimir_lista_expresiones(ListaExpresiones listaValores);

void _imprimir_expresion(Expresion expresion) {
    switch (expresion.tipo) {
        case EXP_VALOR:
            _imprimir_valor(expresion.valor);
            break;
        case EXP_IDENTIFICADOR:
            printf("%s", string_a_puntero(&expresion.identificador));
            break;
        case EXP_OP_LLAMADA:
            _imprimir_expresion(*(Expresion*)expresion.llamadaFuncion.funcion);
            _imprimir_lista_expresiones(expresion.llamadaFuncion.argumentos);
            break;
        case EXP_OP_ASIGNACION:
            printf("%s = ", string_a_puntero(&expresion.asignacion.identificador));
            _imprimir_expresion(*(Expresion*)expresion.asignacion.expresion);
            break;
        case EXP_OP_DEF_FUNCION:
            printf("\\");
            _imprimir_lista_identificadores(expresion.defFuncion.argumentos);
            printf(" => ");
            _imprimir_expresion(* (Expresion*) expresion.defFuncion.cuerpo);
            break;
    }
}

void imprimir_expresion(Expresion expresion) {
    _imprimir_expresion(expresion);
    printf("\n");
}

void _imprimir_lista_expresiones(ListaExpresiones listaExpresiones) {
    printf("[");
    for (int i = 0; i < listaExpresiones.longitud; ++i) {
        _imprimir_expresion(((Expresion*) listaExpresiones.valores)[i]);
        printf(", ");
    }
    printf("]");
}

void imprimir_lista_expresiones(ListaExpresiones listaExpresiones) {
    _imprimir_lista_expresiones(listaExpresiones);
    printf("\n");
}

Expresion crear_exp_valor(Valor valor) {
    return (Expresion) {
        .tipo = EXP_VALOR,
        .valor = valor,
        .es_sentencia = 0,
    };
}

Expresion crear_exp_identificador(String identificador) {
    return (Expresion) {
            .tipo = EXP_IDENTIFICADOR,
            .identificador = identificador,
            .es_sentencia = 0,
    };
}

Expresion crear_exp_llamada(Expresion funcion, ListaExpresiones argumentos)  {
    Expresion* e = malloc(sizeof (Expresion));
    *e = funcion;

    return (Expresion) {
            .tipo = EXP_OP_LLAMADA,
            .llamadaFuncion = (LlamadaFuncion) {
                    .funcion = (struct Expresion *) e,
                    .argumentos = argumentos
            },
            .es_sentencia = 0,
    };
}

Expresion crear_exp_op_unaria(String operador, Expresion x) {
    ListaExpresiones args = crear_lista_expresiones();
    push_lista_expresiones(&args, x);
    return crear_exp_llamada(crear_exp_identificador(operador), args);
}

Expresion crear_exp_op_binaria(String operador, Expresion a, Expresion b) {
    ListaExpresiones args = crear_lista_expresiones();
    push_lista_expresiones(&args, a);
    push_lista_expresiones(&args, b);
    return crear_exp_llamada(crear_exp_identificador(operador), args);
}

Expresion crear_exp_asignacion(String identificador, Expresion expresion, int inmutable) {
    Expresion* e = malloc(sizeof (Expresion));
    *e = expresion;

    return (Expresion) {
            .tipo = EXP_OP_ASIGNACION,
            .asignacion = (Asignacion) {
                    .identificador = identificador,
                    .expresion = (struct Expresion*) e,
                    .inmutable = inmutable
            },
            .es_sentencia = 0,
    };
}

Expresion crear_exp_def_funcion(ListaIdentificadores argumentos, Expresion cuerpo) {
    Expresion* e = malloc(sizeof (Expresion));
    *e = cuerpo;

    return (Expresion) {
            .tipo = EXP_OP_DEF_FUNCION,
            .defFuncion = (DefinicionFuncion) {
                    .argumentos = argumentos,
                    .cuerpo = (struct Expresion*) e
            },
            .es_sentencia = 0,
    };
}

Expresion crear_exp_bloque(ListaExpresiones expresiones) {
    return (Expresion) {
        .tipo = EXP_BLOQUE,
        .bloque = expresiones,
        .es_sentencia = 0,
    };
}

ListaExpresiones crear_lista_expresiones() {
    return (ListaExpresiones) {
        .longitud = 0,
        .capacidad = 0,
        .valores = NULL
    };
}

void push_lista_expresiones(ListaExpresiones *lista, Expresion expresion) {
    if (lista->longitud >= lista->capacidad) {
        lista->valores = realloc(lista->valores, (lista->capacidad+1)*sizeof(Expresion));
        ++lista->capacidad;
    }
    ((Expresion*) lista->valores)[lista->longitud++] = expresion;
}

ListaIdentificadores crear_lista_identificadores() {
    return (ListaIdentificadores) {
            .longitud = 0,
            .capacidad = 0,
            .valores = NULL
    };
}

void push_lista_identificadores(ListaIdentificadores *lista, String identificador) {
    if (lista->longitud >= lista->capacidad) {
        lista->valores = realloc(lista->valores, (lista->capacidad+1)*sizeof(String));
        ++lista->capacidad;
    }
    ((String *) lista->valores)[lista->longitud++] = identificador;
}

