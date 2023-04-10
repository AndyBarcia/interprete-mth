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
            _imprimir_lista_expresiones(expresion.bloque.lista);
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
            for (int i = 0; i < expresion.bloque.lista.longitud; ++i) {
                Expresion subexpresion = ((Expresion *) expresion.bloque.lista.valores)[i];
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

Expresion crear_exp_acceso(Expresion valor, Identificador miembro, Localizacion *loc) {
    Expresion *e = malloc(sizeof(Expresion));
    *e = valor;

    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }

    return (Expresion) {
        .tipo = EXP_ACCESO_MIEMBRO,
        .acceso = (AccesoMiembro) {
            .valor = (struct Expresion*) e,
            .miembro = miembro,
            .loc = loc
        }
    };
}

Expresion crear_exp_llamada(Expresion funcion, ListaExpresiones argumentos, Localizacion *loc) {
    Expresion *e = malloc(sizeof(Expresion));
    *e = funcion;

    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }

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

Expresion crear_exp_op_unaria(FuncionIntrinseca op, Localizacion *opLoc, Expresion x, Localizacion *loc) {
    ListaExpresiones args = crear_lista_expresiones(NULL);
    push_lista_expresiones(&args, x);
    return crear_exp_llamada(crear_exp_valor(crear_funcion_intrinseca(op, opLoc)), args, loc);
}

Expresion crear_exp_op_binaria(FuncionIntrinseca op, Localizacion *opLoc, Expresion a, Expresion b, Localizacion *loc) {
    ListaExpresiones args = crear_lista_expresiones(NULL);
    push_lista_expresiones(&args, a);
    push_lista_expresiones(&args, b);
    return crear_exp_llamada(crear_exp_valor(crear_funcion_intrinseca(op, opLoc)), args, loc);
}

Expresion crear_exp_asignacion(Identificador identificador, Expresion expresion, TipoAsignacion asignacion, Localizacion *loc) {
    Expresion *e = malloc(sizeof(Expresion));
    *e = expresion;

    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }

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

Expresion crear_exp_def_funcion(ListaIdentificadores argumentos, Expresion cuerpo, Localizacion *loc) {
    Expresion *e = malloc(sizeof(Expresion));
    *e = cuerpo;

    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }

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

Expresion crear_exp_bloque(ListaExpresiones expresiones, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }

    return (Expresion) {
            .tipo = EXP_BLOQUE,
            .bloque = (BloqueExpresiones) {
                .lista = expresiones,
                .loc = loc
            },
            .es_sentencia = 0,
    };
}

Expresion crear_exp_importe(String archivo, int foraneo, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }

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

Expresion crear_exp_importe_as(String archivo, int foraneo, Identificador as, Localizacion *loc) {
    Identificador *as_p = malloc(sizeof(Identificador));
    *as_p = as;

    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }

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

Expresion crear_exp_ctrl_flujo(TipoControlFlujo tipo, Expresion *retorno, Localizacion *loc) {
    if (retorno) {
        Expresion* retorno_copy = malloc(sizeof(Expresion));
        *retorno_copy = *(Expresion*) retorno;
        retorno = retorno_copy;
    }

    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }

    return (Expresion) {
        .tipo = EXP_CONTROL_FLUJO,
        .control_flujo = (ControlFlujo) {
            .tipo = tipo,
            .retorno = (struct Expresion*) retorno,
            .loc = loc
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
            e.nombre = clonar_identificador(exp.nombre);
            break;
        case EXP_OP_LLAMADA:
            e.llamada_funcion.funcion = malloc(sizeof(Expresion));
            *(Expresion *) e.llamada_funcion.funcion = clonar_expresion(*(Expresion *) exp.llamada_funcion.funcion);
            e.llamada_funcion.args = clonar_lista_expresiones(exp.llamada_funcion.args);
            if (e.llamada_funcion.loc) {
                e.llamada_funcion.loc = malloc(sizeof(Localizacion));
                *e.llamada_funcion.loc = *exp.llamada_funcion.loc;
            }
            break;
        case EXP_OP_ASIGNACION:
            e.asignacion.identificador = clonar_identificador(exp.asignacion.identificador);
            e.asignacion.expresion = malloc(sizeof(Expresion)); // Y esto??
            *(Expresion *) e.asignacion.expresion = clonar_expresion(*(Expresion *) exp.asignacion.expresion);
            if (e.asignacion.loc) {
                e.asignacion.loc = malloc(sizeof(Localizacion));
                *e.asignacion.loc = *exp.asignacion.loc;
            }
            break;
        case EXP_OP_DEF_FUNCION:
            e.def_funcion.nombres_args = clonar_lista_identificadores(exp.def_funcion.nombres_args);
            e.def_funcion.cuerpo = malloc(sizeof(Expresion)); // Esto es necesario
            *(Expresion *) e.def_funcion.cuerpo = clonar_expresion(*(Expresion *) exp.def_funcion.cuerpo);
            if (e.def_funcion.loc) {
                e.def_funcion.loc = malloc(sizeof(Localizacion));
                *e.def_funcion.loc = *exp.def_funcion.loc;
            }
            break;
        case EXP_BLOQUE:
            e.bloque.lista = clonar_lista_expresiones(exp.bloque.lista);
            if (e.bloque.loc) {
                e.bloque.loc = malloc(sizeof(Localizacion));
                *e.bloque.loc = *exp.bloque.loc;
            }
            break;
        case EXP_IMPORT:
            e.importe.archivo = clonar_string(exp.importe.archivo);
            if (e.importe.as) {
                e.importe.as = malloc(sizeof(Identificador));
                *e.importe.as = clonar_identificador(*exp.importe.as);
            }
            if (e.importe.loc) {
                e.importe.loc = malloc(sizeof(Localizacion));
                *e.importe.loc = *exp.importe.loc;
            }
            break;
        case EXP_ACCESO_MIEMBRO:
            e.acceso.miembro = clonar_identificador(exp.acceso.miembro);
            e.acceso.valor = malloc(sizeof(Expresion));
            *(Expresion *) e.acceso.valor = clonar_expresion(*(Expresion *) exp.acceso.valor);
            if (e.acceso.loc) {
                e.acceso.loc = malloc(sizeof(Localizacion));
                *e.acceso.loc = *exp.acceso.loc;
            }
            break;
        case EXP_CONTROL_FLUJO:
            if (e.control_flujo.retorno) {
                e.control_flujo.retorno = malloc(sizeof(Expresion));
                *(Expresion*) e.control_flujo.retorno = clonar_expresion(*(Expresion*) exp.control_flujo.retorno);
            }
            if (e.control_flujo.loc) {
                e.control_flujo.loc = malloc(sizeof(Localizacion));
                *e.control_flujo.loc = *exp.control_flujo.loc;
            }
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
            if(exp->llamada_funcion.loc) free(exp->llamada_funcion.loc);
            break;
        case EXP_OP_ASIGNACION:
            borrar_expresion((Expresion *) exp->asignacion.expresion);
            free(exp->asignacion.expresion);
            borrar_string(&exp->asignacion.identificador.nombre);
            if(exp->asignacion.loc) free(exp->asignacion.loc);
            break;
        case EXP_OP_DEF_FUNCION:
            borrar_expresion((Expresion *) exp->def_funcion.cuerpo);
            free(exp->def_funcion.cuerpo);
            borrar_lista_identificadores(&exp->def_funcion.nombres_args);
            if(exp->def_funcion.loc) free(exp->def_funcion.loc);
            break;
        case EXP_BLOQUE:
            borrar_lista_expresiones(&exp->bloque.lista);
            if (exp->bloque.loc) free(exp->bloque.loc);
            break;
        case EXP_IMPORT:
            borrar_string(&exp->importe.archivo);
            if (exp->importe.as) {
                borrar_identificador(exp->importe.as);
                free(exp->importe.as);
            }
            if(exp->importe.loc) free(exp->importe.loc);
            break;
        case EXP_ACCESO_MIEMBRO:
            borrar_string(&exp->acceso.miembro.nombre);
            borrar_expresion((Expresion*) exp->acceso.valor);
            free(exp->acceso.valor);
            if(exp->acceso.loc) free(exp->acceso.loc);
            break;
        case EXP_CONTROL_FLUJO:
            if (exp->control_flujo.retorno) {
                borrar_expresion((Expresion *) exp->control_flujo.retorno);
                free(exp->control_flujo.retorno);
            }
            if(exp->control_flujo.loc) free(exp->control_flujo.loc);
            break;
    }
    exp->tipo = EXP_NULA;
}

Localizacion* obtener_loc_exp(Expresion *exp) {
    switch (exp->tipo) {
        case EXP_NULA:
            return NULL;
        case EXP_VALOR:
            return exp->valor.loc;
        case EXP_IDENTIFICADOR:
            return &exp->nombre.loc;
        case EXP_ACCESO_MIEMBRO:
            return exp->acceso.loc;
        case EXP_OP_LLAMADA:
            return exp->llamada_funcion.loc;
        case EXP_OP_ASIGNACION:
            return exp->asignacion.loc;
        case EXP_OP_DEF_FUNCION:
            return exp->def_funcion.loc;
        case EXP_BLOQUE:
            return exp->bloque.loc;
        case EXP_IMPORT:
            return exp->importe.loc;
        case EXP_CONTROL_FLUJO:
            return exp->control_flujo.loc;
    }
}

ListaExpresiones crear_lista_expresiones(Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }

    return (ListaExpresiones) {
            .longitud = 0,
            .capacidad = 0,
            .valores = NULL,
            .loc = loc
    };
}

ListaExpresiones crear_lista_expresiones1(Expresion expresion) {
    ListaExpresiones lista = crear_lista_expresiones(obtener_loc_exp(&expresion));
    push_lista_expresiones(&lista, expresion);
    return lista;
}

void push_lista_expresiones(ListaExpresiones *lista, Expresion expresion) {
    if (lista->longitud >= lista->capacidad) {
        lista->valores = realloc(lista->valores, (lista->capacidad + 1) * sizeof(Expresion));
        ++lista->capacidad;
    }
    ((Expresion *) lista->valores)[lista->longitud++] = expresion;

    Localizacion *loc_exp = obtener_loc_exp(&expresion);
    if (loc_exp && !lista->loc) {
        lista->loc = malloc(sizeof(Localizacion));
        *lista->loc = *loc_exp;
    } else if (loc_exp) {
        lista->loc->last_line = loc_exp->last_line;
        lista->loc->last_column = loc_exp->last_column;
    }
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
    if (lista->loc) {
        free(lista->loc);
        lista->loc = NULL;
    }
    free(lista->valores);
    lista->capacidad = 0;
    lista->longitud = 0;
}