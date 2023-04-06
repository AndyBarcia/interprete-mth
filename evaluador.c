#include "evaluador.h"
#include "analizador_sintactico.h"
#include "analizador_lexico.h"

Evaluador crear_evaluador(Lexer lexer) {
    return (Evaluador) {
        .lexer = lexer,
        .ps = yypstate_new(),
        .loc = {
                .first_line = 1,
                .last_line = 1,
                .first_column = 1,
                .last_column = 1
        },
    };
}

void borrar_evaluador(Evaluador *evaluador) {
    yypstate_delete((yypstate*) evaluador->ps);
    borrar_analizador_lexico(evaluador->lexer);
}

int evaluar_siguiente(Evaluador *evaluador, TablaSimbolos *tablaSimbolos, Valor *valor) {
    int status;
    do {
        YYSTYPE token;
        int c = siguiente_componente_lexico(evaluador->lexer, &token, &evaluador->loc);

        Expresion exp = crear_exp_nula();
        status = yypush_parse((yypstate*) evaluador->ps, c, &token, (YYLTYPE*) &evaluador->loc, &exp);

        if (exp.tipo != EXP_NULA) {
            *valor = evaluar_expresion(tablaSimbolos, &exp);
            return 1;
        }
    } while (status == YYPUSH_MORE);
    return 0;
}

// Evalúa el valor de la expresión, liberando toda la memoria asociada
// a la expresión antes de terminar.
Valor evaluar_expresion(TablaSimbolos *tabla, Expresion *exp) {
    // Hay que tener en cuenta que sólo hay que devolver un valor
    // si la expresión no es una sentencia. Si no, se tiene que
    // devolver "indefinido".
    // Si en algún momento nos encontramos con un valor de tipo
    // "error", hay que abortar y devolver ese valor, aunque la
    // expresión sea una sentencia.
    switch (exp->tipo) {
        case EXP_VALOR: {
            // La expresión es un valor; devolverlo si no es una sentencia.
            if (exp->es_sentencia && exp->valor.tipoValor != TIPO_ERROR)
                return crear_indefinido();
            return exp->valor;
        }
        case EXP_IDENTIFICADOR: {
            // Si la expresión es un identificador; buscar su valor en la tabla de símbolos.
            Valor v = recuperar_valor_tabla(*tabla, exp->identificador);
            if (v.tipoValor == TIPO_ERROR) return v;
            borrar_string(&exp->identificador.nombre);

            if (exp->es_sentencia) return crear_indefinido();
            return v;
        }
        case EXP_OP_LLAMADA: {
            // Si es una llamada, evaluar la expresión que queremos llamar como función,
            // y comprobar que efectivamente es una función, y distinguir entre si es
            // una función nativa o si es una función definida por el usuario.
            Valor f = evaluar_expresion(tabla, (Expresion*)exp->llamadaFuncion.funcion);
            free(exp->llamadaFuncion.funcion);
            switch (f.tipoValor) {
                case TIPO_ERROR:
                    borrar_lista_expresiones(&exp->llamadaFuncion.argumentos);
                    return f;
                case TIPO_FUNCION_NATIVA: {
                    FuncionNativa fn = f.funcion_nativa;
                    ListaValores args = evaluar_expresiones(tabla, &exp->llamadaFuncion.argumentos);

                    for(int i = 0; i < args.longitud; ++i) {
                        Valor v = ((Valor*)args.valores)[i];
                        if (v.tipoValor == TIPO_ERROR) {
                            // Borrar todos los valores menos el que vamos a devolver.
                            for (int j = 0; j < args.longitud; ++j) {
                                if (j != i) borrar_valor(&((Valor*)args.valores)[j]);
                            }
                            free(args.valores);
                            return v;
                        }
                    }

                    Valor result = crear_indefinido();
                    fn(tabla, args, &result);
                    borrar_lista_valores(&args);
                    borrar_valor(&f);

                    if (exp->es_sentencia) return crear_indefinido();
                    return result;
                }
                case TIPO_FUNCION: {
                    Funcion fn = f.funcion;
                    ListaValores args = evaluar_expresiones(tabla, &exp->llamadaFuncion.argumentos);

                    if (fn.argumentos.longitud != args.longitud) {
                        borrar_lista_valores(&args);
                        Error error = crear_error("Se pasaron %d argumentos, pero se esperaban %d.", args.longitud, fn.argumentos.longitud);
                        return crear_valor_error(error, f.loc);
                    }

                    // Introducir los argumentos en la tabla de símbolos.
                    aumentar_nivel_tabla_simbolos(tabla);
                    for (int i = 0; i < args.longitud; ++i) {
                        Valor arg = ((Valor*)args.valores)[i];
                        if (arg.tipoValor == TIPO_ERROR) {
                            reducir_nivel_tabla_simbolos(tabla);
                            // Borrar todos los valores menos los que ya metimos en la
                            // tabla y menos el que vamos a devolver.
                            for (int j = i+1; j < args.longitud; ++j)
                                borrar_valor(&((Valor*)args.valores)[j]);
                            free(args.valores);
                            return arg;
                        }
                        asignar_valor_tabla(tabla, clonar_identificador(fn.argumentos.valores[i]), arg, 0);
                    }
                    // Introducir las variables capturadas por la función
                    asignar_clones_valores_tabla(tabla, *(TablaHash *) fn.variables_capturadas);

                    Expresion cuerpo = clonar_expresion(*(Expresion*)fn.cuerpo);
                    Valor v = evaluar_expresion(tabla, &cuerpo);

                    reducir_nivel_tabla_simbolos(tabla);
                    free(args.valores);
                    borrar_valor(&f);

                    if (exp->es_sentencia) return crear_indefinido();
                    return v;
                }
                default: {
                    return crear_valor_error(crear_error("Este valor no es una función!"), f.loc);
                }
            }
        }
        case EXP_OP_ASIGNACION: {
            Valor v = evaluar_expresion(tabla, (Expresion*)exp->asignacion.expresion);
            free(exp->asignacion.expresion);
            if (v.tipoValor == TIPO_ERROR) {
                borrar_string(&exp->asignacion.identificador.nombre);
                return v;
            }
            if (asignar_valor_tabla(tabla, exp->asignacion.identificador, clonar_valor(v), exp->asignacion.inmutable)) {
                if (exp->es_sentencia) return crear_indefinido();
                return v;
            } else {
                Error error = crear_error(
                        "Intentando reasignar variable inmutable \"%s\"",
                        string_a_puntero(&exp->asignacion.identificador.nombre));
                borrar_string(&exp->asignacion.identificador.nombre);
                return crear_valor_error(error, &exp->asignacion.identificador.loc);
            }
        }
        case EXP_OP_DEF_FUNCION: {
            Expresion *cuerpo = (Expresion*) exp->defFuncion.cuerpo;
            ListaIdentificadores ids = variables_capturadas(exp->defFuncion);

            TablaHash *capturadas = malloc(sizeof(TablaHash));
            *capturadas = crear_tabla_hash(ids.longitud);

            for(int i = 0; i < ids.longitud; ++i) {
                Valor v = recuperar_valor_tabla(*tabla, ids.valores[i]);
                if (v.tipoValor == TIPO_ERROR) {
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
            return crear_funcion(exp->defFuncion.argumentos, (struct Expresion*) cuerpo, (struct TablaHash*) capturadas, &exp->defFuncion.loc);
        }
        case EXP_BLOQUE: {
            aumentar_nivel_tabla_simbolos(tabla);
            ListaExpresiones lista = exp->bloque;
            Valor ultimo_valor = crear_indefinido();
            for (int i = 0; i < lista.longitud; ++i) {
                borrar_valor(&ultimo_valor);
                ultimo_valor = evaluar_expresion(tabla, &((Expresion*) lista.valores)[i]);
                if (ultimo_valor.tipoValor == TIPO_ERROR) {
                    for (int j = i+1; j < lista.longitud; ++j)
                        borrar_expresion(&((Expresion*) lista.valores)[j]);
                    free(lista.valores);
                    return ultimo_valor;
                }
            }
            reducir_nivel_tabla_simbolos(tabla);

            free(lista.valores);
            if (exp->es_sentencia) return crear_indefinido();
            return ultimo_valor;
        }
        default: {
            borrar_expresion(exp);
            return crear_valor_error(crear_error("Expresión desconocida. What?"), NULL);
        }
    }

}

ListaValores evaluar_expresiones(TablaSimbolos *tabla, ListaExpresiones *listaExpresiones) {
    ListaValores l = {
        .longitud = listaExpresiones->longitud,
        .valores = malloc(listaExpresiones->longitud*sizeof(Valor))
    };
    for (int i = 0; i < l.longitud; ++i)
        ((Valor*)l.valores)[i] = evaluar_expresion(tabla, &((Expresion*)listaExpresiones->valores)[i]);
    free(listaExpresiones->valores);
    listaExpresiones->longitud = 0;
    listaExpresiones->capacidad = 0;
    return l;
}