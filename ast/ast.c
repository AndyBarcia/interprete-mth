#include "ast.h"
#include "../tabla_simbolos.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void _imprimir_lista_expresiones(ListaExpresiones listaValores);
void _imprimir_lista_identificadores(ListaIdentificadores listaIdentificadores);

void _imprimir_expresion(Expresion expresion) {
    switch (expresion.tipo) {
        case EXP_VALOR:
            _imprimir_valor(expresion.valor);
            break;
        case EXP_IDENTIFICADOR:
            printf("%s", string_a_puntero(&expresion.nombre.nombre));
            break;
        case EXP_OP_LLAMADA:
            _imprimir_expresion(*(Expresion *) expresion.llamada_funcion.funcion);
            _imprimir_lista_expresiones(expresion.llamada_funcion.args);
            break;
        case EXP_OP_ASIGNACION:
            printf("%s = ", string_a_puntero(&expresion.asignacion.identificador.nombre));
            _imprimir_expresion(*(Expresion *) expresion.asignacion.expresion);
            break;
        case EXP_OP_DEF_FUNCION:
            printf("\\");
            _imprimir_lista_identificadores(expresion.def_funcion.nombres_args);
            printf(" => ");
            _imprimir_expresion(*(Expresion *) expresion.def_funcion.cuerpo);
            break;
        case EXP_NULA:
            printf("<null>");
            break;
        case EXP_BLOQUE:
            _imprimir_lista_expresiones(expresion.bloque);
            break;
        case EXP_ACCESO_MIEMBRO:
            // TODO:
            break;
        case EXP_IMPORT:
            // TODO:
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
            if (!es_miembro_hash(*locales, string_a_puntero(&expresion.nombre.nombre)))
                push_lista_identificadores(lista, expresion.nombre);
            break;
        case EXP_OP_LLAMADA:
            _variables_capturadas(*(Expresion *) expresion.llamada_funcion.funcion, locales, lista);
            for (int i = 0; i < expresion.llamada_funcion.args.longitud; ++i)
                _variables_capturadas(((Expresion *) expresion.llamada_funcion.args.valores)[i], locales, lista);
            break;
        case EXP_OP_ASIGNACION:
            insertar_hash(locales, expresion.asignacion.identificador.nombre, crear_indefinido(), 0);
            _variables_capturadas(*(Expresion *) expresion.asignacion.expresion, locales, lista);
            break;
        case EXP_OP_DEF_FUNCION:
            for (int i = 0; i < expresion.def_funcion.nombres_args.longitud; ++i)
                insertar_hash(locales, expresion.def_funcion.nombres_args.valores[i].nombre, crear_indefinido(), 0);
            _variables_capturadas(*(Expresion *) expresion.def_funcion.cuerpo, locales, lista);
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
    TablaHash locales = crear_tabla_hash(funcion.nombres_args.longitud);
    for (int i = 0; i < funcion.nombres_args.longitud; ++i)
        insertar_hash(&locales, funcion.nombres_args.valores[i].nombre, crear_indefinido(), 0);

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

Expresion crear_exp_nombre(NombreAsignable nombre) {
    return (Expresion) {EXP_IDENTIFICADOR, .nombre = nombre, .es_sentencia = 0};
}

Expresion crear_exp_acceso(Expresion valor, Identificador miembro, Localizacion loc) {
    Expresion *e = malloc(sizeof(Expresion));
    *e = valor;

    return (Expresion) {
        .tipo = EXP_ACCESO_MIEMBRO,
        .acceso = (AccesoMiembro) {
            .valor = (struct Expresion*) e,
            .miembro = miembro,
            .loc = loc
        }
    };
}

Expresion crear_exp_llamada(Expresion funcion, ListaExpresiones argumentos, Localizacion loc) {
    Expresion *e = malloc(sizeof(Expresion));
    *e = funcion;

    return (Expresion) {
            .tipo = EXP_OP_LLAMADA,
            .llamada_funcion = (LlamadaFuncion) {
                    .funcion = (struct Expresion *) e,
                    .args = argumentos,
                    .loc = loc
            },
            .es_sentencia = 0,
    };
}

Expresion crear_exp_op_unaria(Identificador operador, Expresion x, Localizacion loc) {
    ListaExpresiones args = crear_lista_expresiones();
    push_lista_expresiones(&args, x);
    return crear_exp_llamada(crear_exp_nombre(operador), args, loc);
}

Expresion crear_exp_op_binaria(Identificador operador, Expresion a, Expresion b, Localizacion loc) {
    ListaExpresiones args = crear_lista_expresiones();
    push_lista_expresiones(&args, a);
    push_lista_expresiones(&args, b);
    return crear_exp_llamada(crear_exp_nombre(operador), args, loc);
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
            .def_funcion = (DefinicionFuncion) {
                    .nombres_args = argumentos,
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
                    .as = NULL,
                    .loc = loc,
            },
            .es_sentencia = 0
    };
}

Expresion crear_exp_importe_as(String archivo, int foraneo, Identificador as, Localizacion loc) {
    Identificador *as_p = malloc(sizeof(Identificador));
    *as_p = as;

    return (Expresion) {
        .tipo = EXP_IMPORT,
        .importe = (Import) {
            .archivo = archivo,
            .foraneo = foraneo,
            .as = as_p,
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
            e.nombre.nombre = clonar_string(exp.nombre.nombre);
            break;
        case EXP_OP_LLAMADA:
            e.llamada_funcion.funcion = malloc(sizeof(Expresion));
            *(Expresion *) e.llamada_funcion.funcion = clonar_expresion(*(Expresion *) exp.llamada_funcion.funcion);
            e.llamada_funcion.args = clonar_lista_expresiones(exp.llamada_funcion.args);
            break;
        case EXP_OP_ASIGNACION:
            e.asignacion.identificador = clonar_identificador(exp.asignacion.identificador);
            e.asignacion.expresion = malloc(sizeof(Expresion)); // Y esto??
            *(Expresion *) e.asignacion.expresion = clonar_expresion(*(Expresion *) exp.asignacion.expresion);
            break;
        case EXP_OP_DEF_FUNCION:
            e.def_funcion.nombres_args = clonar_lista_identificadores(exp.def_funcion.nombres_args);
            e.def_funcion.cuerpo = malloc(sizeof(Expresion)); // Esto es necesario
            *(Expresion *) e.def_funcion.cuerpo = clonar_expresion(*(Expresion *) exp.def_funcion.cuerpo);
            break;
        case EXP_BLOQUE:
            e.bloque = clonar_lista_expresiones(exp.bloque);
            break;
        case EXP_IMPORT:
            e.importe.archivo = clonar_string(exp.importe.archivo);
            if (e.importe.as) {
                e.importe.as = malloc(sizeof(Identificador));
                *e.importe.as = clonar_identificador(*exp.importe.as);
            }
            break;
        case EXP_ACCESO_MIEMBRO:
            e.acceso.miembro = clonar_identificador(exp.acceso.miembro);
            e.acceso.valor = malloc(sizeof(Expresion));
            *(Expresion *) e.acceso.valor = clonar_expresion(*(Expresion *) exp.acceso.valor);
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
            borrar_string(&exp->nombre.nombre);
            break;
        case EXP_OP_LLAMADA:
            borrar_expresion((Expresion *) exp->llamada_funcion.funcion);
            free(exp->llamada_funcion.funcion);
            borrar_lista_expresiones(&exp->llamada_funcion.args);
            break;
        case EXP_OP_ASIGNACION:
            borrar_expresion((Expresion *) exp->asignacion.expresion);
            free(exp->asignacion.expresion);
            borrar_string(&exp->asignacion.identificador.nombre);
            break;
        case EXP_OP_DEF_FUNCION:
            borrar_expresion((Expresion *) exp->def_funcion.cuerpo);
            free(exp->def_funcion.cuerpo);
            borrar_lista_identificadores(&exp->def_funcion.nombres_args);
            break;
        case EXP_BLOQUE:
            borrar_lista_expresiones(&exp->bloque);
            break;
        case EXP_IMPORT:
            borrar_string(&exp->importe.archivo);
            if (exp->importe.as) {
                borrar_identificador(exp->importe.as);
                free(exp->importe.as);
            }
            break;
        case EXP_ACCESO_MIEMBRO:
            borrar_string(&exp->acceso.miembro.nombre);
            borrar_expresion((Expresion*) exp->acceso.valor);
            free(exp->acceso.valor);
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

ListaExpresiones crear_lista_expresiones1(Expresion expresion) {
    ListaExpresiones lista = crear_lista_expresiones();
    push_lista_expresiones(&lista, expresion);
    return lista;
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