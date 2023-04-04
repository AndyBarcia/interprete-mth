#include "evaluador.h"
#include "analizador_sintactico.h"
#include "analizador_lexico.h"

Valor _evaluar_con_lexer(TablaSimbolos *tabla, Lexer lexer, int interactivo) {
    if (interactivo) printf("> ");

    Valor ultimo_valor;
    int status;
    yypstate *ps = yypstate_new();
    do {
        YYSTYPE yylval_param;
        int c = siguiente_componente_lexico(lexer, &yylval_param);

        Expresion exp;
        exp.tipo = -1;
        status = yypush_parse(ps, c, &yylval_param, &exp);

        if (exp.tipo != -1) {
            ultimo_valor= evaluar_expresion(tabla, exp);
            if (ultimo_valor.tipoValor == TIPO_ERROR || interactivo) imprimir_valor(ultimo_valor);
            if (interactivo) printf("> ");
        }
    } while (status == YYPUSH_MORE);
    yypstate_delete (ps);
    borrar_analizador_lexico(lexer);

    return ultimo_valor;
}

void evaluar_fichero(TablaSimbolos *tabla, FILE *entrada) {
    Lexer lexer = crear_lexer_fichero(entrada);
    _evaluar_con_lexer(tabla, lexer, entrada == stdin);
}

Valor evaluar_archivo(TablaSimbolos *tabla, char* archivo) {
    FILE* entrada;
    if ((entrada = fopen(archivo, "r")) == NULL) {
        return crear_error("No se pudo abrir el archivo \"%s\".", archivo);
    }
    Lexer lexer = crear_lexer_fichero(entrada);
    _evaluar_con_lexer(tabla, lexer, 0);
    fclose(entrada);
    return crear_indefinido();
}

Valor evaluar_str(TablaSimbolos *tabla, char* str) {
    Lexer lexer = crear_lexer_str(str);
    return _evaluar_con_lexer(tabla, lexer, 0);
}

Valor evaluar_expresion(TablaSimbolos *tabla, Expresion exp) {
    switch (exp.tipo) {
        case EXP_VALOR: {
            if (exp.es_sentencia) return crear_indefinido();
            return exp.valor;
        }
        case EXP_IDENTIFICADOR: {
            Valor v = recuperar_valor_tabla(*tabla, exp.identificador);
            if (v.tipoValor == TIPO_ERROR) return v;
            if (exp.es_sentencia) return crear_indefinido();
            return v;
        }
        case EXP_OP_LLAMADA: {
            Valor f = evaluar_expresion(tabla, *(Expresion*)exp.llamadaFuncion.funcion);
            if (f.tipoValor == TIPO_ERROR) return f;
            switch (f.tipoValor) {
                case TIPO_ERROR:
                    return f;
                case TIPO_INDEFINIDO:
                    return crear_error("Llamando una función no definida!");
                case TIPO_FUNCION_NATIVA: {
                    FuncionNativa fn = f.funcion_nativa;
                    ListaValores args = evaluar_expresiones(tabla, exp.llamadaFuncion.argumentos);
                    for(int i = 0; i < args.longitud; ++i) {
                        if (((Valor*)args.valores)[i].tipoValor == TIPO_ERROR)
                            return ((Valor*)args.valores)[i];
                    }

                    Valor result = crear_indefinido();
                    fn(tabla, args, &result);
                    if (exp.es_sentencia) return crear_indefinido();
                    return result;
                }
                case TIPO_FUNCION: {
                    Funcion fn = f.funcion;
                    ListaValores args = evaluar_expresiones(tabla, exp.llamadaFuncion.argumentos);
                    if (fn.argumentos.longitud != args.longitud)
                        return crear_error("Se pasaron %d argumentos, pero se esperaban %d.", args.longitud, fn.argumentos.longitud);

                    // Introducir los argumentos en la tabla de símbolos.
                    aumentar_nivel_tabla_simbolos(tabla);
                    for (int i = 0; i < args.longitud; ++i) {
                        Valor arg = ((Valor*)args.valores)[i];
                        if (arg.tipoValor == TIPO_ERROR)
                            return arg;
                        asignar_valor_tabla(tabla, fn.argumentos.valores[i], arg, 0);
                    }
                    // Introducir las variables capturadas por la función
                    asignar_valores_tabla(tabla, *(TablaHash*) fn.variables_capturadas);

                    Valor v = evaluar_expresion(tabla, *(Expresion*)fn.cuerpo);
                    reducir_nivel_tabla_simbolos(tabla);

                    if (exp.es_sentencia) return crear_indefinido();
                    return v;
                }
                default:
                    return crear_error("Este valor no es una función!");
            }
        }
        case EXP_OP_ASIGNACION: {
            Valor v = evaluar_expresion(tabla, *(Expresion*)exp.asignacion.expresion);
            if (v.tipoValor == TIPO_ERROR) return v;
            if (asignar_valor_tabla(tabla, exp.asignacion.identificador, v, exp.asignacion.inmutable)) {
                if (exp.es_sentencia) return crear_indefinido();
                return v;
            } else {
                return crear_error("Intentando reasignar variable inmutable \"%s\"", string_a_puntero(&exp.asignacion.identificador));
            }
        }
        case EXP_OP_DEF_FUNCION: {
            Expresion cuerpo = * (Expresion*) exp.defFuncion.cuerpo;
            ListaIdentificadores ids = variables_capturadas(exp.defFuncion);

            TablaHash *capturadas = malloc(sizeof(TablaHash));
            *capturadas = crear_tabla_hash(ids.longitud);

            for(int i = 0; i < ids.longitud; ++i) {
                Valor v = recuperar_valor_tabla(*tabla, ids.valores[i]);
                if (v.tipoValor == TIPO_ERROR) return v;
                insertar_hash(capturadas, ids.valores[i], v, 1);
            }

            return crear_funcion(exp.defFuncion.argumentos, cuerpo, (struct TablaHash*) capturadas);
        }
        case EXP_BLOQUE: {
            aumentar_nivel_tabla_simbolos(tabla);
            ListaExpresiones lista = exp.bloque;
            Valor ultimo_valor = crear_indefinido();
            for (int i = 0; i < lista.longitud; ++i) {
                ultimo_valor = evaluar_expresion(tabla, ((Expresion*) lista.valores)[i]);
                if (ultimo_valor.tipoValor == TIPO_ERROR)
                    return ultimo_valor;
            }
            reducir_nivel_tabla_simbolos(tabla);

            if (exp.es_sentencia) return crear_indefinido();
            return ultimo_valor;
        }
        default:
            return crear_error("What?");
    }

}

ListaValores evaluar_expresiones(TablaSimbolos *tabla, ListaExpresiones listaExpresiones) {
    ListaValores l = {
        .longitud = listaExpresiones.longitud,
        .valores = malloc(listaExpresiones.longitud*sizeof(Valor))
    };
    for (int i = 0; i < l.longitud; ++i)
        ((Valor*)l.valores)[i] = evaluar_expresion(tabla, ((Expresion*)listaExpresiones.valores)[i]);
    return l;
}