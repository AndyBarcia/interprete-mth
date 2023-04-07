#include "ast.h"
#include "tabla_simbolos.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void _imprimir_lista_expresiones(ListaExpresiones listaValores);

void _imprimir_expresion(Expresion expresion) {
    switch (expresion.tipo) {
        case EXP_VALOR:
            _imprimir_valor(expresion.valor);
            break;
        case EXP_IDENTIFICADOR:
            printf("%s", string_a_puntero(&expresion.identificador.nombre));
            break;
        case EXP_OP_LLAMADA:
            _imprimir_expresion(*(Expresion *) expresion.llamadaFuncion.funcion);
            _imprimir_lista_expresiones(expresion.llamadaFuncion.argumentos);
            break;
        case EXP_OP_ASIGNACION:
            printf("%s = ", string_a_puntero(&expresion.asignacion.identificador.nombre));
            _imprimir_expresion(*(Expresion *) expresion.asignacion.expresion);
            break;
        case EXP_OP_DEF_FUNCION:
            printf("\\");
            _imprimir_lista_identificadores(expresion.defFuncion.argumentos);
            printf(" => ");
            _imprimir_expresion(*(Expresion *) expresion.defFuncion.cuerpo);
            break;
        case EXP_NULA:
            printf("<null>");
            break;
        case EXP_BLOQUE:
            _imprimir_lista_expresiones(expresion.bloque);
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
        _imprimir_expresion(((Expresion *) listaExpresiones.valores)[i]);
        printf(", ");
    }
    printf("]");
}

void imprimir_lista_expresiones(ListaExpresiones listaExpresiones) {
    _imprimir_lista_expresiones(listaExpresiones);
    printf("\n");
}

void _variables_capturadas(Expresion expresion, TablaHash *locales, ListaIdentificadores *lista) {
    switch (expresion.tipo) {
        case EXP_VALOR:
        case EXP_NULA:
            break;
        case EXP_IDENTIFICADOR:
            if (!es_miembro_hash(*locales, string_a_puntero(&expresion.identificador.nombre)))
                push_lista_identificadores(lista, expresion.identificador);
            break;
        case EXP_OP_LLAMADA:
            _variables_capturadas(*(Expresion *) expresion.llamadaFuncion.funcion, locales, lista);
            for (int i = 0; i < expresion.llamadaFuncion.argumentos.longitud; ++i)
                _variables_capturadas(((Expresion *) expresion.llamadaFuncion.argumentos.valores)[i], locales, lista);
            break;
        case EXP_OP_ASIGNACION:
            insertar_hash(locales, expresion.asignacion.identificador.nombre, crear_indefinido(), 0);
            _variables_capturadas(*(Expresion *) expresion.asignacion.expresion, locales, lista);
            break;
        case EXP_OP_DEF_FUNCION:
            for (int i = 0; i < expresion.defFuncion.argumentos.longitud; ++i)
                insertar_hash(locales, expresion.defFuncion.argumentos.valores[i].nombre, crear_indefinido(), 0);
            _variables_capturadas(*(Expresion *) expresion.defFuncion.cuerpo, locales, lista);
            break;
        case EXP_BLOQUE:
            for (int i = 0; i < expresion.bloque.longitud; ++i) {
                Expresion subexpresion = ((Expresion *) expresion.bloque.valores)[i];
                _variables_capturadas(subexpresion, locales, lista);
            }
            break;
    }
}

ListaIdentificadores variables_capturadas(DefinicionFuncion funcion) {
    ListaIdentificadores capturadas = crear_lista_identificadores();
    TablaHash locales = crear_tabla_hash(funcion.argumentos.longitud);
    for (int i = 0; i < funcion.argumentos.longitud; ++i)
        insertar_hash(&locales, funcion.argumentos.valores[i].nombre, crear_indefinido(), 0);

    Expresion cuerpo = *(Expresion *) funcion.cuerpo;
    _variables_capturadas(cuerpo, &locales, &capturadas);
    borrar_tabla_hash(&locales);
    return capturadas;
}

Expresion crear_exp_nula() {
    return (Expresion) {EXP_NULA};
}

Expresion crear_exp_valor(Valor valor) {
    return (Expresion) {EXP_VALOR, .valor = valor, .es_sentencia = 0};
}

Expresion crear_exp_identificador(Identificador identificador) {
    return (Expresion) {EXP_IDENTIFICADOR, .identificador = identificador, .es_sentencia = 0};
}

Expresion crear_exp_llamada(Expresion funcion, ListaExpresiones argumentos, Localizacion loc) {
    Expresion *e = malloc(sizeof(Expresion));
    *e = funcion;

    return (Expresion) {
            .tipo = EXP_OP_LLAMADA,
            .llamadaFuncion = (LlamadaFuncion) {
                    .funcion = (struct Expresion *) e,
                    .argumentos = argumentos,
                    .loc = loc
            },
            .es_sentencia = 0,
    };
}

Expresion crear_exp_op_unaria(Identificador operador, Expresion x, Localizacion loc) {
    ListaExpresiones args = crear_lista_expresiones();
    push_lista_expresiones(&args, x);
    return crear_exp_llamada(crear_exp_identificador(operador), args, loc);
}

Expresion crear_exp_op_binaria(Identificador operador, Expresion a, Expresion b, Localizacion loc) {
    ListaExpresiones args = crear_lista_expresiones();
    push_lista_expresiones(&args, a);
    push_lista_expresiones(&args, b);
    return crear_exp_llamada(crear_exp_identificador(operador), args, loc);
}

Expresion crear_exp_asignacion(Identificador identificador, Expresion expresion, TipoAsignacion asignacion, Localizacion loc) {
    Expresion *e = malloc(sizeof(Expresion));
    *e = expresion;

    return (Expresion) {
            .tipo = EXP_OP_ASIGNACION,
            .asignacion = (Asignacion) {
                    .identificador = identificador,
                    .expresion = (struct Expresion *) e,
                    .tipo = asignacion,
                    .loc = loc,
            },
            .es_sentencia = 0,
    };
}

Expresion crear_exp_def_funcion(ListaIdentificadores argumentos, Expresion cuerpo, Localizacion loc) {
    Expresion *e = malloc(sizeof(Expresion));
    *e = cuerpo;

    return (Expresion) {
            .tipo = EXP_OP_DEF_FUNCION,
            .defFuncion = (DefinicionFuncion) {
                    .argumentos = argumentos,
                    .cuerpo = (struct Expresion *) e,
                    .loc = loc
            },
            .es_sentencia = 0,
    };
}

Expresion crear_exp_bloque(ListaExpresiones expresiones, Localizacion loc) {
    return (Expresion) {
            .tipo = EXP_BLOQUE,
            .bloque = expresiones,
            .es_sentencia = 0,
    };
}

Expresion crear_exp_importe(String archivo, int foraneo, Localizacion loc) {
    return (Expresion) {
        .tipo = EXP_IMPORT,
        .importe = (Import) {
            .archivo = archivo,
            .foraneo = foraneo,
            .loc = loc,
        },
        .es_sentencia = 0
    };
}

Expresion clonar_expresion(Expresion exp) {
    Expresion e = exp;
    switch (e.tipo) {
        case EXP_NULA:
            break;
        case EXP_VALOR:
            e.valor = clonar_valor(exp.valor);
            break;
        case EXP_IDENTIFICADOR:
            e.identificador.nombre = clonar_string(exp.identificador.nombre);
            break;
        case EXP_OP_LLAMADA:
            e.llamadaFuncion.funcion = malloc(sizeof(Expresion));
            *(Expresion *) e.llamadaFuncion.funcion = clonar_expresion(*(Expresion *) exp.llamadaFuncion.funcion);
            e.llamadaFuncion.argumentos = clonar_lista_expresiones(exp.llamadaFuncion.argumentos);
            break;
        case EXP_OP_ASIGNACION:
            e.asignacion.identificador = clonar_identificador(exp.asignacion.identificador);
            e.asignacion.expresion = malloc(sizeof(Expresion)); // Y esto??
            *(Expresion *) e.asignacion.expresion = clonar_expresion(*(Expresion *) exp.asignacion.expresion);
            break;
        case EXP_OP_DEF_FUNCION:
            e.defFuncion.argumentos = clonar_lista_identificadores(exp.defFuncion.argumentos);
            e.defFuncion.cuerpo = malloc(sizeof(Expresion)); // Esto es necesario
            *(Expresion *) e.defFuncion.cuerpo = clonar_expresion(*(Expresion *) exp.defFuncion.cuerpo);
            break;
        case EXP_BLOQUE:
            e.bloque = clonar_lista_expresiones(exp.bloque);
            break;
        case EXP_IMPORT:
            e.importe.archivo = clonar_string(e.importe.archivo);
            break;
    }
    return e;
}

void borrar_expresion(Expresion *exp) {
    switch (exp->tipo) {
        case EXP_NULA:
            break;
        case EXP_VALOR:
            borrar_valor(&exp->valor);
            break;
        case EXP_IDENTIFICADOR:
            borrar_string(&exp->identificador.nombre);
            break;
        case EXP_OP_LLAMADA:
            borrar_expresion((Expresion *) exp->llamadaFuncion.funcion);
            free(exp->llamadaFuncion.funcion);
            borrar_lista_expresiones(&exp->llamadaFuncion.argumentos);
            break;
        case EXP_OP_ASIGNACION:
            borrar_expresion((Expresion *) exp->asignacion.expresion);
            free(exp->asignacion.expresion);
            borrar_string(&exp->asignacion.identificador.nombre);
            break;
        case EXP_OP_DEF_FUNCION:
            borrar_expresion((Expresion *) exp->defFuncion.cuerpo);
            free(exp->defFuncion.cuerpo);
            borrar_lista_identificadores(&exp->defFuncion.argumentos);
            break;
        case EXP_BLOQUE:
            borrar_lista_expresiones(&exp->bloque);
            break;
        case EXP_IMPORT:
            borrar_string(&exp->importe.archivo);
            break;
    }
    exp->tipo = EXP_NULA;
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
        lista->valores = realloc(lista->valores, (lista->capacidad + 1) * sizeof(Expresion));
        ++lista->capacidad;
    }
    ((Expresion *) lista->valores)[lista->longitud++] = expresion;
}

ListaExpresiones clonar_lista_expresiones(ListaExpresiones lista) {
    ListaExpresiones result = {
            .capacidad = lista.longitud,
            .longitud = lista.longitud,
            .valores = malloc(sizeof(Expresion) * lista.longitud)
    };
    for (int i = 0; i < lista.longitud; ++i)
        ((Expresion *) result.valores)[i] = clonar_expresion(((Expresion *) lista.valores)[i]);
    return result;
}

void borrar_lista_expresiones(ListaExpresiones *lista) {
    for (int i = 0; i < lista->longitud; ++i)
        borrar_expresion(&((Expresion *) lista->valores)[i]);
    free(lista->valores);
    lista->capacidad = 0;
    lista->longitud = 0;
}