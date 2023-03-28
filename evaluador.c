#include "evaluador.h"

Valor evaluar_expresion(TablaSimbolos *tabla, Expresion exp) {
    switch (exp.tipo) {
        case EXP_VALOR:
            return exp.valor;
        case EXP_IDENTIFICADOR:
            return recuperar_valor_tabla(*tabla, exp.identificador);
        case EXP_OP_UNARIA: {
            Valor v = evaluar_expresion(tabla, *(Expresion *) exp.operacionUnaria.valor);
            if (v.tipoValor == TIPO_ERROR) return v;

            switch (exp.operacionUnaria.operador) {
                case '!':
                    return crear_error(crear_string("Operación no soportada!"));
                default:
                    return crear_error(crear_string("Operador no reconocido!"));
            }
        }
        case EXP_OP_BINARIA: {
            Valor izq = evaluar_expresion(tabla, *(Expresion *) exp.operacionBinaria.izq);
            if (izq.tipoValor == TIPO_ERROR) return izq;
            Valor der = evaluar_expresion(tabla, *(Expresion *) exp.operacionBinaria.der);
            if (der.tipoValor == TIPO_ERROR) return der;

            switch (exp.operacionBinaria.operador) {
                case '+': {
                    return crear_entero(izq.entero + der.entero);
                }
                default:
                    return crear_error(crear_string("Operador no reconocido!"));
            }
        }
        case EXP_OP_LLAMADA: {
            Valor f = recuperar_valor_tabla(*tabla, exp.llamadaFuncion.identificador_funcion);
            switch (f.tipoValor) {
                case TIPO_ERROR:
                    return f;
                case TIPO_INDEFINIDO:
                    return crear_error(crear_string("Función no definida!"));
                case TIPO_FUNCION_NATIVA: {
                    FuncionNativa fn = f.funcion_nativa;
                    ListaValores args = evaluar_expresiones(tabla, exp.llamadaFuncion.argumentos);
                    for(int i = 0; i < args.longitud; ++i) {
                        if (((Valor*)args.valores)[i].tipoValor == TIPO_ERROR)
                            return ((Valor*)args.valores)[i];
                    }

                    Valor result = crear_indefinido();
                    fn(args, &result);
                    return result;
                }
                case TIPO_FUNCION: {
                    Funcion fn = f.funcion;
                    ListaValores args = evaluar_expresiones(tabla, exp.llamadaFuncion.argumentos);
                    if (fn.argumentos.longitud != args.longitud)
                        return crear_error(crear_string("Número incorrecto de argumentos."));
                    for(int i = 0; i < args.longitud; ++i) {
                        if (((Valor*)args.valores)[i].tipoValor == TIPO_ERROR)
                            return ((Valor*)args.valores)[i];
                    }

                    // Introducir los argumentos en la tabla de símbolos-
                    aumentar_nivel_tabla_simbolos(tabla);
                    for (int i = 0; i < args.longitud; ++i)
                        asignar_valor_tabla(tabla, fn.argumentos.valores[i], ((Valor*)args.valores)[i]);
                    Valor v = evaluar_expresion(tabla, *(Expresion*)fn.cuerpo);
                    reducir_nivel_tabla_simbolos(tabla);
                    return v;
                }
                default:
                    return crear_error(crear_string("No es una función!"));
            }
        }
        case EXP_OP_ASIGNACION: {
            Valor v = evaluar_expresion(tabla, *(Expresion*)exp.asignacion.expresion);
            if (v.tipoValor != TIPO_ERROR) {
                asignar_valor_tabla(tabla, exp.asignacion.identificador, v);
                return crear_indefinido();
            } else {
                return v;
            }
        }
        case EXP_BLOQUE: {
            aumentar_nivel_tabla_simbolos(tabla);
            ListaExpresiones lista = exp.bloque;
            Valor ultimo_valor = crear_indefinido();
            for (int i = 0; i < lista.longitud; ++i)
                ultimo_valor = evaluar_expresion(tabla, ((Expresion*) lista.valores)[i]);
            reducir_nivel_tabla_simbolos(tabla);
            return ultimo_valor;
        }
        default:
            return crear_error(crear_string("What?"));
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