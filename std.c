#include "std.h"
#include "ast.h"
#include "evaluador.h"

void sumar(TablaSimbolos *tabla, ListaValores args, Valor *retorno) {
    if (args.longitud == 0) return;

    Valor result = ((Valor*) args.valores)[0];
    for (int i = 1; i < args.longitud; ++i) {
        Valor v = ((Valor *) args.valores)[i];
        if (result.tipoValor == v.tipoValor) {
            switch (result.tipoValor) {
                case TIPO_ENTERO:
                    result.entero += v.entero;
                    break;
                case TIPO_STRING: {
                    String a = result.string;
                    String b = v.string;

                    result.string = crear_string(string_a_puntero(&a));
                    extender_string(&result.string, string_a_puntero(&b));
                    borrar_string(&a);
                    borrar_string(&b);
                    break;
                }
                default:
                    *retorno = crear_valor_error(crear_error("No se pueden sumar valores de este tipo."), result.loc);
                    return;
            }
        } else {
            *retorno = crear_valor_error(crear_error("Sumando valores de tipos distintos."), v.loc);
            return;
        }
    }
    *retorno = result;
}

void restar(TablaSimbolos *tabla, ListaValores args, Valor *retorno) {
    if (args.longitud == 1) {
        Valor v = ((Valor *) args.valores)[0];
        if (v.tipoValor != TIPO_ENTERO) {
            *retorno = crear_valor_error(crear_error("No es de tipo entero."), v.loc);
            return;
        }
        *retorno = crear_entero(-((Valor *) args.valores)[0].entero, v.loc);
    } else if (args.longitud == 2) {
        if (((Valor *) args.valores)[0].tipoValor != TIPO_ENTERO ||
            ((Valor *) args.valores)[1].tipoValor != TIPO_ENTERO) {
            *retorno = crear_valor_error(crear_error("No es de tipo entero."), NULL);
            return;
        }
        *retorno = crear_entero(((Valor *) args.valores)[0].entero - ((Valor *) args.valores)[1].entero, NULL);
    } else {
        *retorno = crear_valor_error(crear_error("Se pasaron argumentos de más."), NULL);
    }
}

void multiplicar(TablaSimbolos *tabla, ListaValores args, Valor *retorno) {
    if (args.longitud < 2) {
        *retorno = crear_valor_error(crear_error("Faltan argumentos."), NULL);
        return;
    }

    int a = 1;
    for (int i = 0; i < args.longitud; ++i) {
        if (((Valor *) args.valores)[i].tipoValor != TIPO_ENTERO) {
            *retorno = crear_valor_error(crear_error("No es de tipo entero."), NULL);
            return;
        }
        a *= ((Valor *) args.valores)[i].entero;
    }
    *retorno = crear_entero(a, NULL);
}

void igualdad(TablaSimbolos *tabla, ListaValores args, Valor *retorno) {
    if (args.longitud < 2) {
        *retorno = crear_valor_error(crear_error("Faltan argumentos."), NULL);
        return;
    }

    Valor v = ((Valor *) args.valores)[0];
    for (int i = 1; i < args.longitud; ++i) {
        if (!comparar_valor(v, ((Valor *) args.valores)[i])) {
            *retorno = crear_bool(0, NULL);
            return;
        }
    }
    *retorno = crear_bool(1, NULL);
}

void print(TablaSimbolos *tabla, ListaValores args, Valor *retorno) {
    for (int i = 0; i < args.longitud; ++i) {
        imprimir_valor(((Valor *) args.valores)[i]);
    }
}

void _imprimir_todo(EntradaTablaHash entrada) {
    printf("\"%s\" := ", string_a_puntero(&entrada.clave));
    imprimir_valor(entrada.valor);
}

void _imprimir_solo_usuario(EntradaTablaHash entrada);

void print_ws(TablaSimbolos *tabla, ListaValores args, Valor *retorno) {
    if (args.longitud > 1) {
        *retorno = crear_valor_error(crear_error("Demasiados argumentos."), NULL);
        return;
    }
    if (args.longitud == 1) {
        Valor arg = ((Valor *) args.valores)[0];
        if (arg.tipoValor != TIPO_BOOL) {
            *retorno = crear_valor_error(crear_error("Se esperaba un booleano."), NULL);
            return;
        }
        if (arg.bool) {
            for (int i = 0; i <= tabla->nivel; ++i)
                iterar_tabla_hash(tabla->tablas[i], _imprimir_todo);
            return;
        }
    }
    for (int i = 0; i <= tabla->nivel; ++i) {
        iterar_tabla_hash(tabla->tablas[i], _imprimir_solo_usuario);
    }
}

void eval(TablaSimbolos *tabla, ListaValores args, Valor *retorno) {
    if (args.longitud > 1) {
        *retorno = crear_valor_error(crear_error("Demasiados argumentos."), NULL);
        return;
    } else if (args.longitud == 0) {
        *retorno = crear_valor_error(crear_error("Se esperaba un string a evaluar."), NULL);
        return;
    }
    Valor arg = ((Valor *) args.valores)[0];
    if (arg.tipoValor != TIPO_STRING) {
        *retorno = crear_valor_error(crear_error("Se esperaba un string a evaluar."), NULL);
        return;
    }

    printf("Temporalmente no implementado"); // TODO import
    exit(0);
    //*retorno = evaluar_str(tabla, string_a_puntero(&arg.string));
}

typedef struct {
    char *nombres[2];
    Valor valor;
    char *texto_ayuda;
} ValorLibreriaEstandar;

void ayuda(TablaSimbolos *tabla, ListaValores args, Valor *retorno);

ValorLibreriaEstandar elementos[] = {
        {
                {"sumar",       "+"},
                {TIPO_FUNCION_NATIVA, .funcion_nativa = sumar},
                "Suma un conjunto de enteros."
        },
        {
                {"restar",      "-"},
                {TIPO_FUNCION_NATIVA, .funcion_nativa = restar},
                "Resta un número a otro, o convierte un número positivo a negativo."
        },
        {
                {"multiplicar", "*"},
                {TIPO_FUNCION_NATIVA, .funcion_nativa = multiplicar},
                "Multiplica un conjunto de enteros."
        },
        {
                {"igualdad",    "=="},
                {TIPO_FUNCION_NATIVA, .funcion_nativa = igualdad},
                "Comprueba si un conjunto de valores son iguales."
        },
        {
                {"print",       "imprimir"},
                {TIPO_FUNCION_NATIVA, .funcion_nativa = print},
                "Imprime un conjunto de valores."
        },
        {
                {"print_ws",    "imprimir_ws"},
                {TIPO_FUNCION_NATIVA, .funcion_nativa = print_ws},
                "Imprime todas las variables del conjunto de trabajo (workspace).\nSi recibe \"verdadero\" como primer "
                "argumento, imprime todas las variables, incluidas las de la librería estándar."
        },
        {
                {"eval",    NULL},
                {TIPO_FUNCION_NATIVA, .funcion_nativa = eval},
                "Evalúa el string que se le pasa como argumento, como si fuera parte del código fuente.\n\nNótese que "
                "esto significa que evaluar strings como \"x=5\" crearán nuevas variables."
        },
        {
                {"help",        "ayuda"},
                {TIPO_FUNCION_NATIVA, .funcion_nativa = ayuda},
                "Imprime la documentación de una función de la librería estándar."
        },
        {
                {"verdadero",   "true"},
                {TIPO_BOOL, .bool = 1},
                "Booleano verdadero."
        },
        {
                {"falso",       "false"},
                {TIPO_BOOL, .bool = 0},
                "Booleano falso."
        }
};

void _imprimir_solo_usuario(EntradaTablaHash entrada) {
    char *nombre = string_a_puntero(&entrada.clave);
    for (int i = 0; i < sizeof(elementos) / sizeof(ValorLibreriaEstandar); ++i) {
        if (strcmp(nombre, elementos[i].nombres[0]) == 0)
            return;
        if (elementos[i].nombres[1] && strcmp(nombre, elementos[i].nombres[1]) == 0)
            return;
    }
    printf("\"%s\" := ", string_a_puntero(&entrada.clave));
    imprimir_valor(entrada.valor);
}

void ayuda(TablaSimbolos *tabla, ListaValores args, Valor *retorno) {
    if (args.longitud > 1) {
        *retorno = crear_valor_error(crear_error("Sólo se acepta un argumento de entrada."), NULL);
        return;
    }
    if (args.longitud == 0) {
        printf("Imprime ayuda sobre la función que se pasa como argumento. Ejemplo: `ayuda(sumar)`.\n");
        return;
    }
    Valor arg = ((Valor *) args.valores)[0];
    for (int i = 0; i < sizeof(elementos) / sizeof(ValorLibreriaEstandar); ++i) {
        if (comparar_valor(arg, elementos[i].valor)) {
            printf("%s\n", elementos[i].texto_ayuda);
            return;
        }
    }
    printf("No hay ayuda para este elemento.\n");
}

void inicializar_libreria_estandar(TablaSimbolos *t) {
    for (int i = 0; i < sizeof(elementos) / sizeof(ValorLibreriaEstandar); ++i) {
        ValorLibreriaEstandar v = elementos[i];
        Identificador id = (Identificador) { .nombre = crear_string(v.nombres[0]) };
        asignar_valor_tabla(t, id, v.valor, 1);
        if (v.nombres[1]) {
            Identificador id_alternativo = (Identificador) { .nombre = crear_string(v.nombres[1]) };
            asignar_valor_tabla(t, id_alternativo, v.valor, 1);
        }
    }
}