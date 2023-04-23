#include "ast.h"
#include "../tabla_simbolos.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Expresion crear_exp_nula() {
    return (Expresion) {EXP_NULA};
}

Expresion crear_exp_valor(Valor valor) {
    return (Expresion) {EXP_VALOR, .valor = valor, .es_sentencia = 0};
}

Expresion crear_exp_nombre(NombreAsignable nombre) {
    return (Expresion) {EXP_NOMBRE, .nombre = nombre, .es_sentencia = 0};
}

Expresion crear_exp_llamada(Expresion funcion, ListaExpresiones argumentos, Localizacion *loc) {
    Expresion *e = malloc(sizeof(Expresion));
    *e = funcion;

    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = clonar_loc(*loc);
        loc = loc_copy;
    }

    return (Expresion) {
            .tipo = EXP_OP_LLAMADA,
            .llamada_funcion = (ExpLlamada) {
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

Expresion crear_exp_asignacion(NombreAsignable nombre, Expresion expresion, Localizacion *loc) {
    Expresion *e = malloc(sizeof(Expresion));
    *e = expresion;

    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = clonar_loc(*loc);
        loc = loc_copy;
    }

    return (Expresion) {
            .tipo = EXP_OP_ASIGNACION,
            .asignacion = (ExpAsignacion) {
                    .nombre = nombre,
                    .expresion = (struct Expresion *) e,
                    .loc = loc,
            },
            .es_sentencia = 0,
    };
}

Expresion crear_exp_definicion(Identificador nombre, Expresion expresion, int export, Localizacion *loc) {
    Expresion *e = malloc(sizeof(Expresion));
    *e = expresion;

    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = clonar_loc(*loc);
        loc = loc_copy;
    }

    return (Expresion) {
            .tipo = EXP_OP_DEFINICION,
            .definicion = (ExpDefinicion) {
                    .nombre = nombre,
                    .expresion = (struct Expresion*) e,
                    .export = export,
                    .loc = loc
            },
            .es_sentencia = 0,
    };
}

// Funciones que calcula la lista de identificadores que una determinada
// definición de función ha capturado del exterior. Estos se determinan
// como aquellos identificadores que no son ni nombres_args de la función
// ni variables locales. Por ejemplo,
//       const five = 5
//       \x => x+five
// Devolvería ["five"] como lista de variables capturadas.
ListaIdentificadores variables_capturadas(ListaIdentificadores nombres_args, Expresion *cuerpo);

Expresion crear_exp_funcion(ListaIdentificadores argumentos, Expresion cuerpo, Localizacion *loc) {
    Expresion *e = malloc(sizeof(Expresion));
    *e = cuerpo;

    ListaIdentificadores capturadas = variables_capturadas(argumentos, e);

    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = clonar_loc(*loc);
        loc = loc_copy;
    }

    return (Expresion) {
            .tipo = EXP_OP_FUNCION,
            .def_funcion = (ExpFuncion) {
                    .nombres_args = argumentos,
                    .variables_capturadas = capturadas,
                    .cuerpo = (struct Expresion *) e,
                    .loc = loc
            },
            .es_sentencia = 0,
    };
}

Expresion crear_exp_bloque(ListaExpresiones expresiones, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = clonar_loc(*loc);
        loc = loc_copy;
    }

    return (Expresion) {
            .tipo = EXP_BLOQUE,
            .bloque = (ExpBloque) {
                .lista = expresiones,
                .loc = loc
            },
            .es_sentencia = 0,
    };
}

Expresion crear_exp_importe(String archivo, int foraneo, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = clonar_loc(*loc);
        loc = loc_copy;
    }

    return (Expresion) {
            .tipo = EXP_IMPORT,
            .importe = (ExpImporte) {
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
        *loc_copy = clonar_loc(*loc);
        loc = loc_copy;
    }

    return (Expresion) {
        .tipo = EXP_IMPORT,
        .importe = (ExpImporte) {
            .archivo = archivo,
            .foraneo = foraneo,
            .as = as_p,
            .loc = loc,
        },
        .es_sentencia = 0
    };
}

Expresion crear_exp_condicional(Expresion condicion, Expresion verdadero, Expresion *falso, Localizacion *loc) {
    Expresion *cp = malloc(sizeof(Expresion));
    *cp = condicion;

    Expresion *vp = malloc(sizeof(Expresion));
    *vp = verdadero;

    if (falso) {
        Expresion *fp = malloc(sizeof(Expresion));
        *fp = *falso;
        falso = fp;
    }

    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = clonar_loc(*loc);
        loc = loc_copy;
    }

    return (Expresion) {
        .tipo = EXP_CONDICIONAL,
        .condicional = (ExpCondicional) {
            .condicion = (struct Expresion*) cp,
            .verdadero = (struct Expresion*) vp,
            .falso = (struct Expresion*) falso,
            .loc = loc
        },
        .es_sentencia = 0
    };
}

Expresion crear_exp_bucle_while(Expresion condicion, Expresion cuerpo, Localizacion *loc) {
    Expresion *cp = malloc(sizeof(Expresion));
    *cp = condicion;

    Expresion *vp = malloc(sizeof(Expresion));
    *vp = cuerpo;

    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = clonar_loc(*loc);
        loc = loc_copy;
    }

    return (Expresion) {
            .tipo = EXP_BUCLE_WHILE,
            .bucle_while = (ExpWhile) {
                .condicion = (struct Expresion*) cp,
                .cuerpo = (struct Expresion*) vp,
                .loc = loc
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
        *loc_copy = clonar_loc(*loc);
        loc = loc_copy;
    }

    return (Expresion) {
        .tipo = EXP_CONTROL_FLUJO,
        .control_flujo = (ExpControlFlujo) {
            .tipo = tipo,
            .retorno = (struct Expresion*) retorno,
            .loc = loc
        },
        .es_sentencia = 0
    };
}

void validar_expresion(Expresion *e, ContextoExpresion ctx) {
    if (!e) return;

    // Dentro de un módulo sólo se permiten imports y asignaciones.
    // Otras expresiones como `5` o `print("hola")` y otras expresiones
    // sin efectos no están permitidas.
    if (ctx.es_modulo && !ctx.es_asignacion && (e->tipo != EXP_IMPORT && e->tipo != EXP_OP_ASIGNACION && e->tipo != EXP_OP_DEFINICION)) {
        Valor v = crear_valor_error(crear_error_contexto_incorrecto("expresión sin efectos", "fuera de una asignación"),
                                    obtener_loc_exp(e));
        borrar_expresion(e);
        *e = crear_exp_valor(v);
        return;
    }

    switch (e->tipo) {
        case EXP_OP_ASIGNACION:
            ctx.es_asignacion = 1;
            validar_expresion((Expresion*) e->asignacion.expresion, ctx);
            break;
        case EXP_OP_DEFINICION:
            // Los exports sólo se permiten en un módulo, y sólo en el
            // primer nivel del módulo. Algo como `x = { export y = 5; }`
            // tampoco se permite.
            if (e->definicion.export && (!ctx.es_modulo || ctx.es_asignacion)) {
                Valor v = crear_valor_error(crear_error_contexto_incorrecto("export", "fuera de un módulo"), e->definicion.loc);
                borrar_expresion(e);
                *e = crear_exp_valor(v);
                return;
            }
            ctx.es_asignacion = 1;
            validar_expresion((Expresion*) e->definicion.expresion, ctx);
            break;
        case EXP_CONTROL_FLUJO:
            // Los `breaks` sólo se permiten si estamos dentro de un bucle.
            if (e->control_flujo.tipo == CTR_FLUJO_BREAK && !ctx.es_bucle) {
                Valor v = crear_valor_error(crear_error_contexto_incorrecto("break", "fuera de un bucle"), e->control_flujo.loc);
                borrar_expresion(e);
                *e = crear_exp_valor(v);
                return;
            // Y los `return` sólo si estamos dentro de la definición de una función.
            } else if (e->control_flujo.tipo == CTR_FLUJO_RETURN && !ctx.es_funcion) {
                Valor v = crear_valor_error(crear_error_contexto_incorrecto("return", "fuera de una función"), e->control_flujo.loc);
                borrar_expresion(e);
                *e = crear_exp_valor(v);
                return;
            }
            validar_expresion((Expresion*) e->control_flujo.retorno, ctx);
            break;
        case EXP_BLOQUE:
            ctx.es_bloque = 1;
            for (int i = 0; i < e->bloque.lista.longitud; ++i)
                validar_expresion(&((Expresion*) e->bloque.lista.valores)[i], ctx);
            break;
        case EXP_CONDICIONAL:
            validar_expresion((Expresion*) e->condicional.condicion, ctx);
            validar_expresion((Expresion*) e->condicional.verdadero, ctx);
            validar_expresion((Expresion*) e->condicional.falso, ctx);
            break;
        case EXP_BUCLE_WHILE:
            validar_expresion((Expresion*) e->bucle_while.condicion, ctx);
            ctx.es_bucle = 1;
            validar_expresion((Expresion*) e->bucle_while.cuerpo, ctx);
            break;
        case EXP_OP_FUNCION:
            ctx.es_funcion = 1;
            validar_expresion((Expresion*) e->def_funcion.cuerpo, ctx);
            break;
        default: break;
    }
}

Expresion clonar_expresion(Expresion exp) {
    Expresion e = exp;
    switch (e.tipo) {
        case EXP_NULA:
            break;
        case EXP_VALOR:
            e.valor = clonar_valor(exp.valor);
            break;
        case EXP_NOMBRE:
            e.nombre = clonar_nombre_asignable(exp.nombre);
            break;
        case EXP_OP_LLAMADA:
            e.llamada_funcion.funcion = malloc(sizeof(Expresion));
            *(Expresion *) e.llamada_funcion.funcion = clonar_expresion(*(Expresion *) exp.llamada_funcion.funcion);
            e.llamada_funcion.args = clonar_lista_expresiones(exp.llamada_funcion.args);
            if (e.llamada_funcion.loc) {
                e.llamada_funcion.loc = malloc(sizeof(Localizacion));
                *e.llamada_funcion.loc = clonar_loc(*exp.llamada_funcion.loc);
            }
            break;
        case EXP_OP_ASIGNACION:
            e.asignacion.nombre = clonar_nombre_asignable(exp.asignacion.nombre);
            e.asignacion.expresion = malloc(sizeof(Expresion));
            *(Expresion *) e.asignacion.expresion = clonar_expresion(*(Expresion *) exp.asignacion.expresion);
            if (e.asignacion.loc) {
                e.asignacion.loc = malloc(sizeof(Localizacion));
                *e.asignacion.loc = clonar_loc(*exp.asignacion.loc);
            }
            break;
        case EXP_OP_DEFINICION:
            e.definicion.nombre = clonar_identificador(exp.definicion.nombre);
            e.definicion.expresion = malloc(sizeof(Expresion));
            *(Expresion *) e.definicion.expresion = clonar_expresion(*(Expresion *) exp.definicion.expresion);
            if (e.definicion.loc) {
                e.definicion.loc = malloc(sizeof(Localizacion));
                *e.definicion.loc = clonar_loc(*exp.definicion.loc);
            }
            break;
        case EXP_OP_FUNCION:
            e.def_funcion.nombres_args = clonar_lista_identificadores(exp.def_funcion.nombres_args);
            e.def_funcion.variables_capturadas = clonar_lista_identificadores(exp.def_funcion.variables_capturadas);
            e.def_funcion.cuerpo = malloc(sizeof(Expresion)); // Esto es necesario
            *(Expresion *) e.def_funcion.cuerpo = clonar_expresion(*(Expresion *) exp.def_funcion.cuerpo);
            if (e.def_funcion.loc) {
                e.def_funcion.loc = malloc(sizeof(Localizacion));
                *e.def_funcion.loc = clonar_loc(*exp.def_funcion.loc);
            }
            break;
        case EXP_BLOQUE:
            e.bloque.lista = clonar_lista_expresiones(exp.bloque.lista);
            if (e.bloque.loc) {
                e.bloque.loc = malloc(sizeof(Localizacion));
                *e.bloque.loc = clonar_loc(*exp.bloque.loc);
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
                *e.importe.loc = clonar_loc(*exp.importe.loc);
            }
            break;
        case EXP_CONDICIONAL:
            e.condicional.condicion = malloc(sizeof(Expresion));
            *(Expresion*) e.condicional.condicion = clonar_expresion(*(Expresion*) exp.condicional.condicion);
            e.condicional.verdadero = malloc(sizeof(Expresion));
            *(Expresion*) e.condicional.verdadero = clonar_expresion(*(Expresion*) exp.condicional.verdadero);
            if (e.condicional.falso) {
                e.condicional.falso = malloc(sizeof(Expresion));
                *(Expresion*) e.condicional.falso = clonar_expresion(*(Expresion*) exp.condicional.falso);
            }
            if (e.condicional.loc) {
                e.condicional.loc = malloc(sizeof(Localizacion));
                *e.condicional.loc = clonar_loc(*exp.condicional.loc);
            }
            break;
        case EXP_BUCLE_WHILE:
            e.bucle_while.condicion = malloc(sizeof(Expresion));
            *(Expresion*) e.bucle_while.condicion = clonar_expresion(*(Expresion*) exp.bucle_while.condicion);
            e.bucle_while.cuerpo = malloc(sizeof(Expresion));
            *(Expresion*) e.bucle_while.cuerpo = clonar_expresion(*(Expresion*) exp.bucle_while.cuerpo);
            if (e.bucle_while.loc) {
                e.bucle_while.loc = malloc(sizeof(Localizacion));
                *e.bucle_while.loc = clonar_loc(*exp.bucle_while.loc);
            }
            break;
        case EXP_CONTROL_FLUJO:
            if (e.control_flujo.retorno) {
                e.control_flujo.retorno = malloc(sizeof(Expresion));
                *(Expresion*) e.control_flujo.retorno = clonar_expresion(*(Expresion*) exp.control_flujo.retorno);
            }
            if (e.control_flujo.loc) {
                e.control_flujo.loc = malloc(sizeof(Localizacion));
                *e.control_flujo.loc = clonar_loc(*exp.control_flujo.loc);
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
        case EXP_NOMBRE:
            borrar_nombre_asignable(&exp->nombre);
            break;
        case EXP_OP_LLAMADA:
            borrar_expresion((Expresion *) exp->llamada_funcion.funcion);
            free(exp->llamada_funcion.funcion);
            borrar_lista_expresiones(&exp->llamada_funcion.args);
            if(exp->llamada_funcion.loc) {
                borrar_loc(exp->llamada_funcion.loc);
                free(exp->llamada_funcion.loc);
            }
            break;
        case EXP_OP_ASIGNACION:
            borrar_expresion((Expresion *) exp->asignacion.expresion);
            free(exp->asignacion.expresion);
            borrar_nombre_asignable(&exp->asignacion.nombre);
            if(exp->asignacion.loc) {
                borrar_loc(exp->asignacion.loc);
                free(exp->asignacion.loc);
            }
            break;
        case EXP_OP_DEFINICION:
            borrar_expresion((Expresion *) exp->definicion.expresion);
            free(exp->definicion.expresion);
            borrar_identificador(&exp->definicion.nombre);
            if(exp->definicion.loc) {
                borrar_loc(exp->definicion.loc);
                free(exp->definicion.loc);
            }
            break;
        case EXP_OP_FUNCION:
            borrar_expresion((Expresion *) exp->def_funcion.cuerpo);
            free(exp->def_funcion.cuerpo);
            borrar_lista_identificadores(&exp->def_funcion.nombres_args);
            borrar_lista_identificadores(&exp->def_funcion.variables_capturadas);
            if(exp->def_funcion.loc) {
                borrar_loc(exp->def_funcion.loc);
                free(exp->def_funcion.loc);
            }
            break;
        case EXP_BLOQUE:
            borrar_lista_expresiones(&exp->bloque.lista);
            if (exp->bloque.loc) {
                borrar_loc(exp->bloque.loc);
                free(exp->bloque.loc);
            }
            break;
        case EXP_IMPORT:
            borrar_string(&exp->importe.archivo);
            if (exp->importe.as) {
                borrar_identificador(exp->importe.as);
                free(exp->importe.as);
            }
            if(exp->importe.loc) {
                borrar_loc(exp->importe.loc);
                free(exp->importe.loc);
            }
            break;
        case EXP_CONDICIONAL:
            borrar_expresion((Expresion*) exp->condicional.condicion);
            free(exp->condicional.condicion);
            borrar_expresion((Expresion*) exp->condicional.verdadero);
            free(exp->condicional.verdadero);
            if (exp->condicional.falso) {
                borrar_expresion((Expresion*) exp->condicional.falso);
                free(exp->condicional.falso);
            }
            if(exp->condicional.loc) {
                borrar_loc(exp->condicional.loc);
                free(exp->condicional.loc);
            }
            break;
        case EXP_BUCLE_WHILE:
            borrar_expresion((Expresion*) exp->bucle_while.condicion);
            free(exp->bucle_while.condicion);
            borrar_expresion((Expresion*) exp->bucle_while.cuerpo);
            free(exp->bucle_while.cuerpo);
            if(exp->bucle_while.loc) {
                borrar_loc(exp->bucle_while.loc);
                free(exp->bucle_while.loc);
            }
            break;
        case EXP_CONTROL_FLUJO:
            if (exp->control_flujo.retorno) {
                borrar_expresion((Expresion *) exp->control_flujo.retorno);
                free(exp->control_flujo.retorno);
            }
            if(exp->control_flujo.loc) {
                borrar_loc(exp->control_flujo.loc);
                free(exp->control_flujo.loc);
            }
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
        case EXP_NOMBRE:
            // TODO Areglar esto
            return &exp->nombre.nombre_base.loc;
        case EXP_OP_LLAMADA:
            return exp->llamada_funcion.loc;
        case EXP_OP_ASIGNACION:
            return exp->asignacion.loc;
        case EXP_OP_DEFINICION:
            return exp->definicion.loc;
        case EXP_OP_FUNCION:
            return exp->def_funcion.loc;
        case EXP_BLOQUE:
            return exp->bloque.loc;
        case EXP_IMPORT:
            return exp->importe.loc;
        case EXP_CONTROL_FLUJO:
            return exp->control_flujo.loc;
        case EXP_CONDICIONAL:
            return exp->condicional.loc;
        case EXP_BUCLE_WHILE:
            return exp->bucle_while.loc;
    }
}

ListaExpresiones crear_lista_expresiones(Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = clonar_loc(*loc);
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
        *lista->loc = clonar_loc(*loc_exp);
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
        borrar_loc(lista->loc);
        free(lista->loc);
        lista->loc = NULL;
    }
    free(lista->valores);
    lista->capacidad = 0;
    lista->longitud = 0;
}

void _imprimir_lista_expresiones(ListaExpresiones listaValores);
void _imprimir_lista_identificadores(ListaIdentificadores listaIdentificadores);
void _imprimir_nombre_asignable(NombreAsignable nombre);
void _imprimir_acceso(Acceso acceso);
void _imprimir_valor(Valor valor);

void _imprimir_expresion(Expresion expresion) {
    switch (expresion.tipo) {
        case EXP_VALOR:
            _imprimir_valor(expresion.valor);
            break;
        case EXP_NOMBRE:
            _imprimir_nombre_asignable(expresion.nombre);
            break;
        case EXP_OP_LLAMADA:
            _imprimir_expresion(*(Expresion *) expresion.llamada_funcion.funcion);
            _imprimir_lista_expresiones(expresion.llamada_funcion.args);
            break;
        case EXP_OP_ASIGNACION:
            _imprimir_nombre_asignable(expresion.asignacion.nombre);
            printf(" = ");
            _imprimir_expresion(*(Expresion*) expresion.asignacion.expresion);
            break;
        case EXP_OP_DEFINICION:
            printf(expresion.definicion.export ? "export " : "const ");
            printf("%s = ", identificador_a_str(&expresion.definicion.nombre));
            _imprimir_expresion(*(Expresion*) expresion.definicion.expresion);
            break;
        case EXP_OP_FUNCION:
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
        case EXP_IMPORT:
            printf("import ");
            if (expresion.importe.foraneo)  printf("foreign ");
            printf("\"%s\"", string_a_str(&expresion.importe.archivo));
            if (expresion.importe.as) {
                printf(" as %s", string_a_str(&expresion.importe.as->nombre));
            }
            break;
        case EXP_CONDICIONAL:
            printf("if ");
            _imprimir_expresion(*(Expresion*) expresion.condicional.condicion);
            printf(" then ");
            _imprimir_expresion(*(Expresion*) expresion.condicional.verdadero);
            if (expresion.condicional.falso) {
                printf(" else ");
                _imprimir_expresion(*(Expresion*) expresion.condicional.falso);
            }
            break;
        case EXP_CONTROL_FLUJO:
            printf("%s", tipo_control_flujo_a_str(expresion.control_flujo.tipo));
            if (expresion.control_flujo.retorno) {
                _imprimir_expresion(*(Expresion*) expresion.control_flujo.retorno);
            }
            break;
        case EXP_BUCLE_WHILE:
            printf("while ");
            _imprimir_expresion(*(Expresion*) expresion.bucle_while.condicion);
            printf(" do ");
            _imprimir_expresion(*(Expresion*) expresion.bucle_while.cuerpo);
            break;
    }
}

void imprimir_expresion(Expresion expresion) {
    printf("`");
    _imprimir_expresion(expresion);
    printf("`\n");
}

void _imprimir_lista_expresiones(ListaExpresiones listaExpresiones) {
    printf("[");
    for (int i = 0; i < listaExpresiones.longitud; ++i) {
        _imprimir_expresion(((Expresion *) listaExpresiones.valores)[i]);
        if (i + 1 < listaExpresiones.longitud)
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
        case EXP_NOMBRE:
            if (!es_miembro_hash(*locales, string_a_str(&expresion.nombre.nombre_base.nombre)))
                push_lista_identificadores(lista, clonar_identificador(expresion.nombre.nombre_base));
            break;
        case EXP_OP_LLAMADA:
            _variables_capturadas(*(Expresion *) expresion.llamada_funcion.funcion, locales, lista);
            for (int i = 0; i < expresion.llamada_funcion.args.longitud; ++i)
                _variables_capturadas(((Expresion *) expresion.llamada_funcion.args.valores)[i], locales, lista);
            break;
        case EXP_OP_ASIGNACION:
            insertar_hash(locales, clonar_string(expresion.asignacion.nombre.nombre_base.nombre), crear_valor_unidad(NULL), 0);
            _variables_capturadas(*(Expresion *) expresion.asignacion.expresion, locales, lista);
            break;
        case EXP_OP_DEFINICION:
            insertar_hash(locales, clonar_string(expresion.definicion.nombre.nombre), crear_valor_unidad(NULL), 0);
            _variables_capturadas(*(Expresion *) expresion.definicion.expresion, locales, lista);
            break;
        case EXP_OP_FUNCION:
            for (int i = 0; i < expresion.def_funcion.nombres_args.longitud; ++i)
                insertar_hash(locales, clonar_string(expresion.def_funcion.nombres_args.valores[i].nombre), crear_valor_unidad(NULL), 0);
            _variables_capturadas(*(Expresion *) expresion.def_funcion.cuerpo, locales, lista);
            break;
        case EXP_BLOQUE:
            for (int i = 0; i < expresion.bloque.lista.longitud; ++i) {
                Expresion subexpresion = ((Expresion *) expresion.bloque.lista.valores)[i];
                _variables_capturadas(subexpresion, locales, lista);
            }
            break;
        case EXP_IMPORT:
            break;
        case EXP_CONDICIONAL:
            _variables_capturadas(*(Expresion *) expresion.condicional.condicion, locales, lista);
            _variables_capturadas(*(Expresion *) expresion.condicional.verdadero, locales, lista);
            if (expresion.condicional.falso)
                _variables_capturadas(*(Expresion *) expresion.condicional.falso, locales, lista);
            break;
        case EXP_BUCLE_WHILE:
            _variables_capturadas(*(Expresion*) expresion.bucle_while.condicion, locales, lista);
            _variables_capturadas(*(Expresion*) expresion.bucle_while.cuerpo, locales, lista);
            break;
        case EXP_CONTROL_FLUJO:
            if (expresion.control_flujo.retorno)
                _variables_capturadas(*(Expresion*) expresion.control_flujo.retorno, locales, lista);
            break;
    }
}

ListaIdentificadores variables_capturadas(ListaIdentificadores nombres_args, Expresion *cuerpo) {
    ListaIdentificadores capturadas = crear_lista_identificadores();
    TablaHash locales = crear_tabla_hash(nombres_args.longitud);
    for (int i = 0; i < nombres_args.longitud; ++i)
        insertar_hash(&locales, clonar_string(nombres_args.valores[i].nombre), crear_valor_unidad(NULL), 0);

    _variables_capturadas(*cuerpo, &locales, &capturadas);
    borrar_tabla_hash(&locales);
    return capturadas;
}
