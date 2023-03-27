#include "evaluar.h"
#include <stdlib.h>
#include <stdio.h>

Valor crear_entero(int entero) {
    return (Valor) {
        .tipoValor = TIPO_ENTERO,
        .entero = entero
    };
}

Valor crear_identificador(String identificador) {
    return (Valor) {
            .tipoValor = TIPO_IDENTIFICADOR,
            .identificador = identificador
    };
}

Valor crear_error(String error) {
    return (Valor) {
            .tipoValor = TIPO_ERROR,
            .error = error
    };
}

void _imprimir_valor(Valor valor) {
    switch (valor.tipoValor) {
        case TIPO_NULO: printf("null"); break;
        case TIPO_ERROR: printf("Error: %s", string_a_puntero(&valor.error)); break;
        case TIPO_ENTERO: printf("%d", valor.entero); break;
        case TIPO_IDENTIFICADOR: printf("%s", string_a_puntero(&valor.identificador)); break;
    }
}

void imprimir_valor(Valor valor) {
    _imprimir_valor(valor);
    printf("\n");
}

void _imprimir_expresion(Expresion expresion) {
    switch (expresion.tipo) {
        case EXP_VALOR:
            _imprimir_valor(expresion.valor);
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
            imprimir_lista_expresiones(*(ListaExpresiones*) expresion.llamadaFuncion.argumentos);
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
    ListaExpresiones* eA = malloc(sizeof (ListaExpresiones));
    *eA = argumentos;

    return (Expresion) {
            .tipo = EXP_OP_LLAMADA,
            .llamadaFuncion = (LlamadaFuncion) {
                    .identificador_funcion = identificador,
                    .argumentos = (struct ListaExpresiones *) eA
            }
    };
}

Enunciado crear_enunciado_exp(Expresion expresion) {
    return (Enunciado) {
        .tipo = ENUNCIADO_EXPRESION,
        .expresion = expresion
    };
}

Enunciado crear_enunciado_asignacion(String identificador, Expresion expresion) {
    return (Enunciado) {
            .tipo = ENUNCIADO_ASIGNACION,
            .asignacion = (Asignacion) {
                .identificador = identificador,
                .valor = expresion
            }
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

    lista->valores[lista->longitud++] = expresion;
}

void _imprimir_lista_expresiones(ListaExpresiones listaValores) {
    printf("[");
    for (int i = 0; i < listaValores.longitud; ++i) {
        _imprimir_expresion(listaValores.valores[i]);
        printf(", ");
    }
    printf("]");
}

void imprimir_lista_expresiones(ListaExpresiones listaValores) {
    _imprimir_lista_expresiones(listaValores);
    printf("\n");
}