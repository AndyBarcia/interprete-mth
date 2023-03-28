#include "evaluador.h"

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
                    for(int i = 0; i < args.longitud; ++i) {
                        if (((Valor*)args.valores)[i].tipoValor == TIPO_ERROR)
                            return ((Valor*)args.valores)[i];
                    }

                    // Introducir los argumentos en la tabla de símbolos-
                    aumentar_nivel_tabla_simbolos(tabla);
                    for (int i = 0; i < args.longitud; ++i)
                        asignar_valor_tabla(tabla, fn.argumentos.valores[i], ((Valor*)args.valores)[i], 0);
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
            // TODO: calcular variables capturadas
            return crear_funcion(exp.defFuncion.argumentos, cuerpo);
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