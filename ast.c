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

Valor crear_entero(int entero) {
    return (Valor) {
        .tipoValor = TIPO_ENTERO,
        .entero = entero
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
            .cuerpo = e
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
    }
}

void imprimir_valor(Valor valor) {
    _imprimir_valor(valor);
    if (valor.tipoValor != TIPO_INDEFINIDO) printf("\n");
}

void _imprimir_expresion(Expresion expresion) {
    switch (expresion.tipo) {
        case EXP_VALOR:
            _imprimir_valor(expresion.valor);
            break;
        case EXP_IDENTIFICADOR:
            printf("%s", string_a_puntero(&expresion.identificador));
            break;
        case EXP_OP_UNARIA:
            printf("(%c)", expresion.operacionUnaria.operador);
            _imprimir_expresion(*(Expresion *) expresion.operacionUnaria.valor);
            break;
        case EXP_OP_BINARIA:
            _imprimir_expresion(*(Expresion *) expresion.operacionBinaria.izq);
            printf("(%c)", expresion.operacionBinaria.operador);
            _imprimir_expresion(*(Expresion *) expresion.operacionBinaria.der);
            break;
        case EXP_OP_LLAMADA:
            printf("%s", string_a_puntero(&expresion.llamadaFuncion.identificador_funcion));
            imprimir_lista_expresiones(expresion.llamadaFuncion.argumentos);
            break;
        case EXP_OP_ASIGNACION:
            printf("%s = ", string_a_puntero(&expresion.asignacion.identificador));
            imprimir_expresion(*(Expresion*)expresion.asignacion.expresion);
            break;
    }
}

void imprimir_expresion(Expresion expresion) {
    _imprimir_expresion(expresion);
    printf("\n");
}

Expresion crear_exp_valor(Valor valor) {
    return (Expresion) {
        .tipo = EXP_VALOR,
        .valor = valor
    };
}

Expresion crear_exp_identificador(String identificador) {
    return (Expresion) {
            .tipo = EXP_IDENTIFICADOR,
            .identificador = identificador
    };
}

Expresion crear_exp_op_unaria(int op, Expresion  exp) {
    Expresion* e = malloc(sizeof (Expresion));
    *e = exp;

    return (Expresion) {
            .tipo = EXP_OP_UNARIA,
            .operacionUnaria = (OperacionUnaria) {
                .operador = op,
                .valor = (struct Expresion *) e
            }
    };
}

Expresion crear_exp_op_binaria(Expresion expA, int op, Expresion expB) {
    Expresion* eA = malloc(sizeof (Expresion));
    *eA = expA;
    Expresion* eB = malloc(sizeof (Expresion));
    *eB = expB;

    return (Expresion) {
            .tipo = EXP_OP_BINARIA,
            .operacionBinaria = (OperacionBinaria) {
                    .izq = (struct Expresion *) eA,
                    .operador = op,
                    .der = (struct Expresion *) eB
            }
    };
}

Expresion crear_exp_llamada(String identificador, ListaExpresiones argumentos)  {
    return (Expresion) {
            .tipo = EXP_OP_LLAMADA,
            .llamadaFuncion = (LlamadaFuncion) {
                    .identificador_funcion = identificador,
                    .argumentos = argumentos
            }
    };
}

Expresion crear_exp_asignacion(String identificador, Expresion expresion) {
    Expresion* e = malloc(sizeof (Expresion));
    *e = expresion;

    return (Expresion) {
            .tipo = EXP_OP_ASIGNACION,
            .asignacion = (Asignacion) {
                    .identificador = identificador,
                    .expresion = (struct Expresion*) e
            }
    };
}

Expresion crear_exp_bloque(ListaExpresiones expresiones) {
    return (Expresion) {
        .tipo = EXP_BLOQUE,
        .bloque = expresiones
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

void _imprimir_lista_expresiones(ListaExpresiones listaValores) {
    printf("[");
    for (int i = 0; i < listaValores.longitud; ++i) {
        _imprimir_expresion(((Expresion*) listaValores.valores)[i]);
        printf(", ");
    }
    printf("]");
}

void imprimir_lista_expresiones(ListaExpresiones listaValores) {
    _imprimir_lista_expresiones(listaValores);
    printf("\n");
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