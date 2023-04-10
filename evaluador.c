#include "evaluador.h"
#include "analizador_sintactico.h"
#include "analizador_lexico.h"
#include "std.h"

Evaluador crear_evaluador(Lexer lexer, Contexto contexto) {
    return (Evaluador) {
        .lexer = lexer,
        .ps = yypstate_new(),
        .contexto = contexto
    };
}

void borrar_evaluador(Evaluador *evaluador) {
    yypstate_delete((yypstate*) evaluador->ps);
    borrar_analizador_lexico(evaluador->lexer);
}

int evaluar_siguiente(Evaluador *evaluador, TablaSimbolos *tabla_simbolos, Valor *valor) {
    int status;
    do {
        YYSTYPE token;
        int c = siguiente_componente_lexico(&evaluador->lexer, &token);

        Expresion exp = crear_exp_nula();
        status = yypush_parse((yypstate*) evaluador->ps, c, &token, (YYLTYPE*) &evaluador->lexer.loc, &exp);

        if (exp.tipo != EXP_NULA) {
            *valor = evaluar_expresion(tabla_simbolos, &exp, evaluador->contexto);
            return 1;
        }
    } while (status == YYPUSH_MORE);
    return 0;
}

// Evalúa el valor de la expresión, liberando toda la memoria asociada
// a la expresión antes de terminar.
Valor evaluar_expresion(TablaSimbolos *tabla, Expresion *exp, Contexto contexto) {
    // Hay que tener en cuenta que sólo hay que devolver un valor
    // si la expresión no es una sentencia. Si no, se tiene que
    // devolver "indefinido".
    // Si en algún momento nos encontramos con un valor de tipo
    // "error", hay que abortar y devolver ese valor, aunque la
    // expresión sea una sentencia.
    switch (exp->tipo) {
        case EXP_VALOR: {
            // La expresión es un valor; devolverlo si no es una sentencia.
            if (exp->es_sentencia && exp->valor.tipo_valor != TIPO_ERROR) {
                borrar_valor(&exp->valor);
                return crear_indefinido();
            }
            return exp->valor;
        }
        case EXP_IDENTIFICADOR: {
            // Si la expresión es un identificador; buscar su valor en la tabla de símbolos.
            Valor v = recuperar_valor_tabla(*tabla, exp->nombre);
            if (v.tipo_valor == TIPO_ERROR) return v;
            borrar_identificador(&exp->nombre);

            if (exp->es_sentencia) {
                borrar_valor(&v);
                return crear_indefinido();
            }
            return v;
        }
        case EXP_ACCESO_MIEMBRO: {
            Valor v = evaluar_expresion(tabla, (Expresion *) exp->acceso.valor, contexto);
            free(exp->acceso.valor);
            if (v.tipo_valor == TIPO_ERROR) return v;

            Valor result = crear_indefinido();
            switch (v.tipo_valor) {
                case TIPO_BIBLIOTECA_FORANEA: {
                    BibilotecaDinamica bib = v.biblioteca;
                    FuncionForanea f = cargar_funcion_biblioteca(bib, identificador_a_str(&exp->acceso.miembro));
                    if (f) {
                        result = crear_funcion_foranea(f);
                    } else {
                        Error error = crear_error("No existe la función foránea \"%s\" en la biblioteca.", identificador_a_str(&exp->acceso.miembro));
                        result = crear_valor_error(error, &exp->acceso.miembro.loc);
                    }
                    break;
                }
                default: {
                    Error error = crear_error("No se puede acceder al miembro \"%s\".", identificador_a_str(&exp->acceso.miembro));
                    result = crear_valor_error(error, &exp->acceso.miembro.loc);
                    break;
                }
            }

            borrar_valor(&v);
            borrar_identificador(&exp->acceso.miembro);
            free(exp->acceso.loc);
            if (exp->es_sentencia && result.tipo_valor != TIPO_ERROR) {
                borrar_valor(&result);
                return crear_indefinido();
            }
            return result;
        }
        case EXP_OP_LLAMADA: {
            // Si es una llamada, evaluar la expresión que queremos llamar como función,
            // y comprobar que efectivamente es una función, y distinguir entre si es
            // una función nativa o si es una función definida por el usuario.
            Valor f = evaluar_expresion(tabla, (Expresion*)exp->llamada_funcion.funcion, contexto);
            free(exp->llamada_funcion.funcion);
            if (f.tipo_valor == TIPO_ERROR) {
                borrar_lista_expresiones(&exp->llamada_funcion.args);
                free(exp->llamada_funcion.loc);
                return f;
            }

            // Obtener la lista de argumentos y abortar si alguno de ellos es un error.
            ListaValores args = evaluar_expresiones(tabla, &exp->llamada_funcion.args, CNTXT_ARGS);
            for(int i = 0; i < args.longitud; ++i) {
                Valor v = ((Valor*)args.valores)[i];
                if (v.tipo_valor == TIPO_ERROR) {
                    // Borrar todos los valores menos el que vamos a devolver.
                    for (int j = 0; j < args.longitud; ++j)
                        if (j != i) borrar_valor(&((Valor*)args.valores)[j]);
                    free(args.valores);
                    free(exp->llamada_funcion.loc);
                    free(exp->llamada_funcion.args.loc);
                    return v;
                }
            }
            free(exp->llamada_funcion.args.loc);

            switch (f.tipo_valor) {
                case TIPO_FUNCION_INTRINSECA: {
                    FuncionIntrinseca fn = f.funcion_intrinseca;
                    Valor result = ejecutar_funcion_intrinseca(fn, args, tabla);
                    borrar_valor(&f);

                    free(exp->llamada_funcion.loc);
                    if (exp->es_sentencia) {
                        borrar_valor(&result);
                        return crear_indefinido();
                    }
                    return result;
                }
                case TIPO_FUNCION_FORANEA: {
                    FuncionForanea fn = f.funcion_foranea;

                    int a = ((Valor*) args.valores)[0].entero;
                    int b = ((Valor*) args.valores)[1].entero;
                    int r = fn(a,b);

                    borrar_lista_valores(&args);
                    borrar_valor(&f);

                    free(exp->llamada_funcion.loc);
                    if (exp->es_sentencia) {
                        return crear_indefinido();
                    }
                    return crear_entero(r, NULL);
                }
                case TIPO_FUNCION: {
                    Funcion fn = f.funcion;

                    if (fn.nombres_args.longitud != args.longitud) {
                        borrar_lista_valores(&args);
                        free(exp->llamada_funcion.loc);
                        Error error = crear_error("Se pasaron %d argumentos, pero se esperaban %d.", args.longitud, fn.nombres_args.longitud);
                        return crear_valor_error(error, exp->llamada_funcion.loc);
                    }

                    // Introducir los nombres_args en la tabla de símbolos.
                    aumentar_nivel_tabla_simbolos(tabla);
                    for (int i = 0; i < args.longitud; ++i) {
                        String nombre = clonar_string(fn.nombres_args.valores[i].nombre);
                        asignar_valor_tabla(tabla, nombre, args.valores[i], ASIGNACION_NORMAL);
                    }
                    // Introducir las variables capturadas por la función
                    asignar_clones_valores_tabla(tabla, *(TablaHash *) fn.variables_capturadas);

                    Expresion cuerpo = clonar_expresion(*(Expresion*)fn.cuerpo);
                    Valor v = evaluar_expresion(tabla, &cuerpo, CNTXT_FUNCION);

                    // Si se devolvió un valor de control de flujo, tenemos que
                    // sacar el valor del elemento de control (o propagarlo en el
                    // caso de ser un control de flujo exit().
                    if (v.tipo_valor == TIPO_CONTROL_FLUJO) {
                        switch (v.control_flujo.tipo) {
                            case CTR_FLUJO_EXIT: break;
                            default:
                                v = *(Valor*) v.control_flujo.valor;
                                free(v.control_flujo.valor);
                        }
                    }

                    reducir_nivel_tabla_simbolos(tabla);
                    free(args.valores);
                    free(exp->llamada_funcion.loc);
                    borrar_valor(&f);

                    if (exp->es_sentencia) {
                        borrar_valor(&v);
                        return crear_indefinido();
                    }
                    return v;
                }
                default: {
                    return crear_valor_error(crear_error("Este valor no es una función!"), f.loc);
                }
            }
        }
        case EXP_OP_ASIGNACION: {
            Valor v = evaluar_expresion(tabla, (Expresion*)exp->asignacion.expresion, CNTXT_ASIGNACION);
            free(exp->asignacion.expresion);

            if (v.tipo_valor == TIPO_ERROR) {
                borrar_identificador(&exp->asignacion.identificador);
                return v;
            }

            // Sólo se permiten hacer exports en el contexto de un módulo.
            if (exp->asignacion.tipo == ASIGNACION_EXPORT && contexto != CNTXT_MODULO) {
                Error error = crear_error_contexto_incorrecto("export", "fuera de un módulo");
                borrar_valor(&v);
                v = crear_valor_error(error, exp->asignacion.loc);
                borrar_identificador(&exp->asignacion.identificador);
                return v;
            }
            free(exp->asignacion.loc);

            // Si la asignación es una sentencia, simplemente devolvemos indefinido.
            // Si no, devolvemos un clon de valor que vamos a insertar en la tabla.
            Valor retorno = exp->es_sentencia ? crear_indefinido() : clonar_valor(v);

            if (asignar_valor_tabla(tabla, exp->asignacion.identificador.nombre, v, exp->asignacion.tipo)) {
                return retorno;
            } else {
                Error error = crear_error(
                        "Intentando reasignar variable inmutable \"%s\"",
                        identificador_a_str(&exp->asignacion.identificador));
                borrar_identificador(&exp->asignacion.identificador);
                return crear_valor_error(error, &exp->asignacion.identificador.loc);
            }
        }
        case EXP_OP_DEF_FUNCION: {
            Expresion *cuerpo = (Expresion*) exp->def_funcion.cuerpo;
            ListaIdentificadores ids = variables_capturadas(exp->def_funcion);

            TablaHash *capturadas = malloc(sizeof(TablaHash));
            *capturadas = crear_tabla_hash(ids.longitud);

            for(int i = 0; i < ids.longitud; ++i) {
                Valor v = recuperar_valor_tabla(*tabla, ids.valores[i]);
                if (v.tipo_valor == TIPO_ERROR) {
                    borrar_expresion(cuerpo);
                    free(cuerpo);
                    borrar_lista_identificadores(&ids);
                    borrar_tabla_hash(capturadas);
                    free(capturadas);
                    return v;
                }
                insertar_hash(capturadas, ids.valores[i].nombre, v, 1);
            }
            borrar_lista_identificadores(&ids);

            Valor funcion = crear_funcion(exp->def_funcion.nombres_args, (struct Expresion*) cuerpo, (struct TablaHash*) capturadas, exp->def_funcion.loc);
            free(exp->def_funcion.loc);

            if (exp->es_sentencia) {
                borrar_valor(&funcion);
                return crear_indefinido();
            } else {
                return funcion;
            }
        }
        case EXP_BLOQUE: {
            aumentar_nivel_tabla_simbolos(tabla);
            ListaExpresiones lista = exp->bloque.lista;
            free(exp->bloque.loc);

            Valor ultimo_valor = crear_indefinido();
            for (int i = 0; i < lista.longitud; ++i) {
                borrar_valor(&ultimo_valor);
                ultimo_valor = evaluar_expresion(tabla, &((Expresion*) lista.valores)[i], contexto);

                // Si nos encontramos un error o un valor de control de flujo,
                // terminar prematuramente sin evaluar el resto de expresiones.
                if (ultimo_valor.tipo_valor == TIPO_ERROR || ultimo_valor.tipo_valor == TIPO_CONTROL_FLUJO) {
                    for (int j = i+1; j < lista.longitud; ++j)
                        borrar_expresion(&((Expresion*) lista.valores)[j]);
                    free(lista.valores);
                    reducir_nivel_tabla_simbolos(tabla);
                    return ultimo_valor;
                }
            }
            reducir_nivel_tabla_simbolos(tabla);

            free(lista.valores);
            if (exp->es_sentencia) {
                borrar_valor(&ultimo_valor);
                return crear_indefinido();
            }
            return ultimo_valor;
        }
        case EXP_IMPORT: {
            if (exp->importe.foraneo) {
                // Importar una biblioteca dinámica de C

                BibilotecaDinamica bib = cargar_biblioteca_dinamica(string_a_puntero(&exp->importe.archivo));
                if (!bib) {
                    Error error = crear_error("No se pudo abrir la biblioteca dinámica.");
                    borrar_string(&exp->importe.archivo);
                    if (exp->importe.as) {
                        borrar_identificador(exp->importe.as);
                        free(exp->importe.as);
                    }

                    Valor v = crear_valor_error(error, exp->importe.loc);
                    free(exp->importe.loc);
                    return v;
                }

                Valor v = crear_valor_biblioteca(bib, &exp->importe.as->loc);
                if (!asignar_valor_tabla(tabla, exp->importe.as->nombre, v, ASIGNACION_INMUTABLE)) {
                    Error error = crear_error("Ya hay un objeto definido con este nombre");
                    borrar_string(&exp->importe.archivo);
                    if (exp->importe.as) {
                        borrar_identificador(exp->importe.as);
                        free(exp->importe.as);
                    }
                    free(exp->importe.loc);
                    return crear_valor_error(error, &exp->importe.as->loc);
                }
            } else {
                // Importar un archivo normal.
                // Crear un nuevo evaluador y evaluar el archivo.

                Lexer lexer;
                if (!crear_lexer_archivo(&lexer, string_a_puntero(&exp->importe.archivo))) {
                    Error error = crear_error("No se pudo abrir el archivo \"%s\"", string_a_puntero(&exp->importe.archivo));
                    borrar_string(&exp->importe.archivo);
                    if (exp->importe.as) {
                        borrar_identificador(exp->importe.as);
                        free(exp->importe.as);
                    }
                    Valor v = crear_valor_error(error, exp->importe.loc);
                    free(exp->importe.loc);
                    return v;
                }

                // Crear un evaluador en contexto de módulo.
                Evaluador evaluador = crear_evaluador(lexer, CNTXT_MODULO);

                aumentar_nivel_tabla_simbolos(tabla);

                Valor x;
                while(evaluar_siguiente(&evaluador, tabla, &x)) {
                    if (x.tipo_valor == TIPO_ERROR) {
                        char* linea = obtener_linea(lexer, x.loc->first_line);
                        imprimir_error(x.error, string_a_puntero(&exp->importe.archivo), linea, x.loc);
                        free(linea);
                    }
                    borrar_valor(&x);
                }
                reducir_nivel_tabla_simbolos(tabla);
            }

            borrar_string(&exp->importe.archivo);
            if (exp->importe.as) {
                borrar_identificador(exp->importe.as);
                free(exp->importe.as);
            }
            free(exp->importe.loc);
            return crear_indefinido();
        }
        case EXP_CONDICIONAL: {
            Valor cond = evaluar_expresion(tabla, (Expresion*)exp->condicional.condicion, contexto);
            free(exp->condicional.condicion);
            if (cond.tipo_valor == TIPO_ERROR) {
                borrar_expresion((Expresion*) exp->condicional.verdadero);
                if (exp->condicional.falso) borrar_expresion((Expresion*) exp->condicional.falso);
                free(exp->condicional.loc);
                return cond;
            }
            if (cond.tipo_valor != TIPO_BOOL) {
                Error error = crear_error("Sólo se pueden utilizar booleanos como condicionales.");
                Valor v = crear_valor_error(error, cond.loc);
                borrar_valor(&cond);
                borrar_expresion((Expresion*) exp->condicional.verdadero);
                if (exp->condicional.falso) borrar_expresion((Expresion*) exp->condicional.falso);
                free(exp->condicional.loc);
                return v;
            }

            if (cond.bool) {
                Valor verdadero = evaluar_expresion(tabla, (Expresion*)exp->condicional.verdadero, contexto);
                if (exp->condicional.falso) borrar_expresion((Expresion*) exp->condicional.falso);
                borrar_valor(&cond);
                free(exp->condicional.loc);

                return verdadero;
            } else if (exp->condicional.falso) {
                Valor falso = evaluar_expresion(tabla, (Expresion*)exp->condicional.falso, contexto);
                borrar_expresion((Expresion*) exp->condicional.verdadero);
                borrar_valor(&cond);
                free(exp->condicional.loc);

                return falso;
            } else {
                borrar_valor(&cond);
                free(exp->condicional.loc);
                return crear_indefinido();
            }
        }
        case EXP_CONTROL_FLUJO: {
            // En una expresión de control de flujo siempre se crea un valor de control de flujo,
            // aunque la expresión sea una sentencia.

            if (exp->control_flujo.tipo == CTR_FLUJO_RETURN && contexto != CNTXT_FUNCION) {
                Error error = crear_error_contexto_incorrecto("return", "fuera de una función");
                Valor v = crear_valor_error(error, exp->control_flujo.loc);
                borrar_expresion(exp);
                return v;
            }
            if (exp->control_flujo.tipo == CTR_FLUJO_BREAK && contexto != CNTXT_BUCLE) {
                Error error = crear_error_contexto_incorrecto("break", "fuera de un bucle");
                Valor v = crear_valor_error(error, exp->control_flujo.loc);
                borrar_expresion(exp);
                return v;
            }

            if (exp->control_flujo.retorno) {
                Valor v = evaluar_expresion(tabla, (Expresion*) exp->control_flujo.retorno, contexto);
                free(exp->control_flujo.retorno);
                return crear_valor_control_flujo(exp->control_flujo.tipo, &v, exp->control_flujo.loc);
            } else {
                return crear_valor_control_flujo(exp->control_flujo.tipo, NULL, exp->control_flujo.loc);
            }
        }
        default: {
            borrar_expresion(exp);
            return crear_valor_error(crear_error("Expresión desconocida. What?"), NULL);
        }
    }

}

ListaValores evaluar_expresiones(TablaSimbolos *tabla, ListaExpresiones *listaExpresiones, Contexto contexto) {
    ListaValores valores = crear_lista_valores();

    for (int i = 0; i < listaExpresiones->longitud; ++i) {
        Valor v = evaluar_expresion(tabla, &((Expresion*)listaExpresiones->valores)[i], contexto);
        push_lista_valores(&valores, v);
    }

    if (valores.loc && listaExpresiones->loc)
        *valores.loc = *listaExpresiones->loc;

    free(listaExpresiones->valores);
    listaExpresiones->longitud = 0;
    listaExpresiones->capacidad = 0;
    return valores;
}