#include "std.h"
#include "ast/ast.h"
#include "evaluador.h"
#include "math.h"

void inicializar_libreria_estandar(TablaSimbolos *t) {
    asignar_valor_tabla(t, crear_string("verdadero"), crear_bool(1, NULL), ASIGNACION_INMUTABLE);
    asignar_valor_tabla(t, crear_string("falso"), crear_bool(0, NULL), ASIGNACION_INMUTABLE);
    asignar_valor_tabla(t, crear_string("pi"), crear_decimal(M_PI, NULL), ASIGNACION_INMUTABLE);
    asignar_valor_tabla(t, crear_string("e"), crear_decimal(M_E, NULL), ASIGNACION_INMUTABLE);

    asignar_valor_tabla(t, crear_string("print"), crear_funcion_intrinseca(INTRINSECA_PRINT, NULL), ASIGNACION_INMUTABLE);
    asignar_valor_tabla(t, crear_string("printws"), crear_funcion_intrinseca(INTRINSECA_PRINTWS, NULL), ASIGNACION_INMUTABLE);
    asignar_valor_tabla(t, crear_string("resetws"), crear_funcion_intrinseca(INTRINSECA_RESETWS, NULL), ASIGNACION_INMUTABLE);
    asignar_valor_tabla(t, crear_string("help"), crear_funcion_intrinseca(INTRINSECA_AYUDA, NULL), ASIGNACION_INMUTABLE);
    asignar_valor_tabla(t, crear_string("load"), crear_funcion_intrinseca(INTRINSECA_CARGAR, NULL), ASIGNACION_INMUTABLE);
    asignar_valor_tabla(t, crear_string("eval"), crear_funcion_intrinseca(INTRINSECA_EVAL, NULL), ASIGNACION_INMUTABLE);
}

#define comprobacion_n_args(n_args, op) \
    if (args.longitud != n_args) {      \
        Valor v = crear_valor_error(crear_error_numero_argumentos(n_args, args.longitud), args.loc); \
        borrar_lista_valores(&args);                                \
        return v; \
    }

#define comprobacion_tipos(a,b, op) \
    if (a.tipo_valor != b.tipo_valor) { \
        Valor v = crear_valor_error(crear_error_tipos_incompatibles(op, a.tipo_valor, b.tipo_valor), args.loc); \
        borrar_lista_valores(&args); \
        return v; \
    }

Valor sumar(Valor a, Valor b) {
    Valor result;
    switch (a.tipo_valor) {
        case TIPO_ENTERO: {
            switch (b.tipo_valor) {
                case TIPO_ENTERO:
                    result = crear_entero(a.entero + b.entero, NULL);
                    break;
                case TIPO_DECIMAL:
                    result = crear_decimal(((Decimal) a.entero) + b.decimal, NULL);
                    break;
                default: {
                    Error error = crear_error_tipos_incompatibles("sumar", a.tipo_valor, b.tipo_valor);
                    result = crear_valor_error(error, NULL);
                    break;
                }
            }
            break;
        }
        case TIPO_DECIMAL: {
            switch (b.tipo_valor) {
                case TIPO_ENTERO:
                    result = crear_decimal(a.decimal + ((Decimal) b.entero), NULL);
                    break;
                case TIPO_DECIMAL:
                    result = crear_decimal(a.decimal + b.decimal, NULL);
                    break;
                default: {
                    Error error = crear_error_tipos_incompatibles("sumar", a.tipo_valor, b.tipo_valor);
                    result = crear_valor_error(error, NULL);
                    break;
                }
            }
            break;
        }
        case TIPO_STRING: {
            switch (b.tipo_valor) {
                case TIPO_STRING: {
                    String x = crear_string(string_a_puntero(&a.string));
                    extender_string(&x, string_a_puntero(&b.string));
                    result = crear_valor_string(x, NULL);
                    break;
                }
                default: {
                    Error error = crear_error_tipos_incompatibles("sumar", a.tipo_valor, b.tipo_valor);
                    result = crear_valor_error(error, NULL);
                    break;
                }
            }
            break;
        }
        default: {
            Error error = crear_error_op_incompatible("sumar", a.tipo_valor);
            result = crear_valor_error(error, NULL);
            break;
        }
    }
    borrar_valor(&a);
    borrar_valor(&b);
    return result;
}

Valor restar(Valor a, Valor b) {
    Valor result;
    switch (a.tipo_valor) {
        case TIPO_ENTERO: {
            switch (b.tipo_valor) {
                case TIPO_ENTERO:
                    result = crear_entero(a.entero - b.entero, NULL);
                    break;
                case TIPO_DECIMAL:
                    result = crear_decimal(((Decimal) a.entero) - b.decimal, NULL);
                    break;
                default: {
                    Error error = crear_error_tipos_incompatibles("restar", a.tipo_valor, b.tipo_valor);
                    result = crear_valor_error(error, NULL);
                    break;
                }
            }
            break;
        }
        case TIPO_DECIMAL: {
            switch (b.tipo_valor) {
                case TIPO_ENTERO:
                    result = crear_decimal(a.decimal - ((Decimal) b.entero), NULL);
                    break;
                case TIPO_DECIMAL:
                    result = crear_decimal(a.decimal - b.decimal, NULL);
                    break;
                default: {
                    Error error = crear_error_tipos_incompatibles("restar", a.tipo_valor, b.tipo_valor);
                    result = crear_valor_error(error, NULL);
                    break;
                }
            }
            break;
        }
        default: {
            Error error = crear_error_op_incompatible("restar", a.tipo_valor);
            result = crear_valor_error(error, NULL);
            break;
        }
    }
    borrar_valor(&a);
    borrar_valor(&b);
    return result;
}

Valor mult(Valor a, Valor b) {
    Valor result;
    switch (a.tipo_valor) {
        case TIPO_ENTERO: {
            switch (b.tipo_valor) {
                case TIPO_ENTERO: {
                    result = crear_entero(a.entero * b.entero, NULL);
                    break;
                }
                case TIPO_DECIMAL: {
                    result = crear_decimal(((Decimal) a.entero) * b.decimal, NULL);
                    break;
                }
                case TIPO_STRING: {
                    String x = crear_string(string_a_puntero(&b.string));
                    for (int i = 1; i < a.entero; ++i)
                        extender_string(&x, string_a_puntero(&b.string));
                    result = crear_valor_string(x, NULL);
                    break;
                }
                default: {
                    result = crear_valor_error(crear_error_tipos_incompatibles("multiplicar", a.tipo_valor, b.tipo_valor), NULL);
                    break;
                };
            }
            break;
        }
        case TIPO_DECIMAL: {
            switch (b.tipo_valor) {
                case TIPO_ENTERO: {
                    result = crear_decimal(a.decimal * ((Decimal) b.entero), NULL);
                    break;
                }
                case TIPO_DECIMAL: {
                    result = crear_decimal(a.decimal * b.decimal, NULL);
                    break;
                }
                default: {
                    result = crear_valor_error(crear_error_tipos_incompatibles("multiplicar", a.tipo_valor, b.tipo_valor), NULL);
                    break;
                };
            }
            break;
        }
        case TIPO_STRING: {
            return mult(b, a);
        }
        default: {
            Error error = crear_error_op_incompatible("sumar", a.tipo_valor);
            result = crear_valor_error(error, NULL);
            break;
        }
    }
    borrar_valor(&a);
    borrar_valor(&b);
    return result;
}

Valor dividir(Valor a, Valor b) {
    Valor result;
    switch (a.tipo_valor) {
        case TIPO_ENTERO: {
            switch (b.tipo_valor) {
                case TIPO_ENTERO:
                    if (b.entero == 0)
                        result = crear_valor_error(crear_error_dividir_entre_cero(), NULL);
                    else
                        result = crear_decimal((Decimal) a.entero / (Decimal) b.entero, NULL);
                    break;
                case TIPO_DECIMAL:
                    if (b.decimal == 0.0)
                        result = crear_valor_error(crear_error_dividir_entre_cero(), NULL);
                    else
                        result = crear_decimal((Decimal) a.entero / b.decimal, NULL);
                    break;
                default: {
                    Error error = crear_error_tipos_incompatibles("dividir", a.tipo_valor, b.tipo_valor);
                    result = crear_valor_error(error, NULL);
                    break;
                }
            }
            break;
        }
        case TIPO_DECIMAL: {
            switch (b.tipo_valor) {
                case TIPO_ENTERO:
                    if (b.entero == 0)
                        result = crear_valor_error(crear_error_dividir_entre_cero(), NULL);
                    else
                        result = crear_decimal(a.decimal / (Decimal) b.entero, NULL);
                    break;
                case TIPO_DECIMAL: {
                    if (b.decimal == 0.0)
                        result = crear_valor_error(crear_error_dividir_entre_cero(), NULL);
                    else
                        result = crear_decimal(a.decimal / b.decimal, NULL);
                    break;
                }
                default: {
                    result = crear_valor_error(crear_error_tipos_incompatibles("dividir", a.tipo_valor, b.tipo_valor), NULL);
                    break;
                };
            }
            break;
        }
        default: {
            Error error = crear_error_op_incompatible("dividir", a.tipo_valor);
            result = crear_valor_error(error, NULL);
            break;
        }
    }
    borrar_valor(&a);
    borrar_valor(&b);
    return result;
}

Valor modulo(Valor a, Valor b) {
    Valor result;
    switch (a.tipo_valor) {
        case TIPO_ENTERO: {
            result = crear_entero(a.entero % b.entero, NULL);
            break;
        }
        default: {
            Error error = crear_error_op_incompatible("módulo", a.tipo_valor);
            result = crear_valor_error(error, NULL);
            break;
        }
    }
    borrar_valor(&a);
    borrar_valor(&b);
    return result;
}

Valor and(Valor a, Valor b) {
    Valor result;
    switch (a.tipo_valor) {
        case TIPO_BOOL: {
            result = crear_bool(a.bool && b.bool, NULL);
            break;
        }
        default: {
            Error error = crear_error_op_incompatible("\"&&\"", a.tipo_valor);
            result = crear_valor_error(error, NULL);
            break;
        }
    }
    borrar_valor(&a);
    borrar_valor(&b);
    return result;
}

Valor or(Valor a, Valor b) {
    Valor result;
    switch (a.tipo_valor) {
        case TIPO_BOOL: {
            result = crear_bool(a.bool || b.bool, NULL);
            break;
        }
        default: {
            Error error = crear_error_op_incompatible("\"||\"", a.tipo_valor);
            result = crear_valor_error(error, NULL);
            break;
        }
    }
    borrar_valor(&a);
    borrar_valor(&b);
    return result;
}

Valor negacion(Valor a) {
    Valor result;
    switch (a.tipo_valor) {
        case TIPO_BOOL: {
            result = crear_bool(!a.bool, NULL);
            break;
        }
        default: {
            Error error = crear_error_op_incompatible("negar", a.tipo_valor);
            result = crear_valor_error(error, NULL);
            break;
        }
    }
    borrar_valor(&a);
    return result;
}

Valor negar(Valor a) {
    Valor result;
    switch (a.tipo_valor) {
        case TIPO_ENTERO: {
            result = crear_entero(-a.entero, NULL);
            break;
        }
        case TIPO_DECIMAL: {
            result = crear_decimal(-a.decimal, NULL);
            break;
        }
        default: {
            Error error = crear_error_op_incompatible("negar", a.tipo_valor);
            result = crear_valor_error(error, NULL);
            break;
        }
    }
    borrar_valor(&a);
    return result;
}

void ayuda(Valor v) {
    switch (v.tipo_valor) {
        case TIPO_ERROR:
            // No debería de pasar nunca.
            break;
        case TIPO_NULO:
            printf("Este es un tipo nulo. No deberías de poder tener acceso a él :D");
            break;
        case TIPO_INDEFINIDO:
            printf("Este es un tipo indefinido; es el valor de todas las expresiones que"
                   "no tienen un valor, como `print(\"hola\")` o `x=5;`.\n");
            break;
        case TIPO_ENTERO:
            printf("Este es un entero; puedes realizar todas las operaciones que te podrías"
                   "esperar, como sumar, restar o multiplicar enteros.\n"
                   "En general, los enteros se promocionan a decimales cuando se realizan "
                   "operaciones entre enteros y decimales.\n");
            break;
        case TIPO_DECIMAL:
            printf("Este es un número decimal; puedes realizar todas las operaciones que te"
                   "podrías esperar, como sumar, restar o multiplicar números.\n");
            break;
        case TIPO_BOOL:
            printf("Este es un número booleano, que puede ser o `verdadero` o `falso`.\n"
                   "Puedes realizar operaciones con ellos como '&&' o '||'.\n");
            break;
        case TIPO_STRING:
            printf("Esta es una cadena de caracteres inmutable.\n"
                   "Puedes realizar algunas operaciones como concatenar cadenas con (+), concatenar "
                   "repetidamente una misma cadena un cierto número de veces con (*), o "
                   "comparar cadenas lexicográficamente con (==), (>), (<), etc.\n");
            break;
        case TIPO_FUNCION:
            printf("Esta es una función que puede ser llamada con ciertos argumentos. Se pueden crear de la forma:\n\n"
                   "\t\t \\a,b,c => exp\n\n"
                   "Donde a,b, y c son los argumentos de la expresión, y lo que está a la derecha del \"=>\" es"
                   "el cuerpo de la función, que puede ser una expresión sencilla, o un bloque de expresiones.\n");
            break;
        case TIPO_FUNCION_FORANEA:
            printf("Esta es una función foránea o externa, de una biblioteca dinámica de C.\n"
                   "Ten cuidado al utilizarla.\n");
            break;
        case TIPO_BIBLIOTECA_FORANEA:
            printf("Esta es una biblioteca dinámica de C.\n"
                   "Puedes utilizar métodos de la biblioteca de la forma `biblioteca.metodo(args)`.\n");
            break;
        case TIPO_FUNCION_INTRINSECA:
            switch (v.funcion_intrinseca) {
                case INTRINSECA_SUMA:
                    printf("Permite sumar dos números, o concatenar dos cadenas de caracteres.\n"); break;
                case INTRINSECA_RESTA:
                    printf("Permite restar dos números.\n"); break;
                case INTRINSECA_MULT:
                    printf("Permite multiplicar dos números, o concatenar repetidamente un string a sí mismo.\n"); break;
                case INTRINSECA_DIV:
                    printf("Permite dividir de forma exacta dos números, siempre devolviendo un decimal.\n"
                           "Para una versión para enters, utiliza el operador (//)."); break;
                case INTRINSECA_MOD:
                    printf("Permite utilizar aritmética modular.\n"); break;
                case INTRINSECA_EQ: printf("Comprueba la igualdad de dos valores.\n"); break;
                case INTRINSECA_NEQ: printf("Comprueba la diferencia de dos valores.\n"); break;
                case INTRINSECA_GE: printf("Comprueba si un valor a es mayor que un valor b.\n"); break;
                case INTRINSECA_GEQ: printf("Comprueba si un valor a es mayor o igual que un valor b.\n"); break;
                case INTRINSECA_LE: printf("Comprueba si un valor a es menor que un valor b.\n"); break;
                case INTRINSECA_LEQ: printf("Comprueba si un valor a es menor o igual que un valor b.\n"); break;
                case INTRINSECA_AND: printf("Comprueba si dos booleanos son verdaderos.\n"); break;
                case INTRINSECA_OR: printf("Comprueba si al menos uno de los booleanos es verdadero.\n"); break;
                case INTRINSECA_NOT: printf("Niega el valor de un booleano.\n"); break;
                case INTRINSECA_NEGAR: printf("Convierte un número en su negativo.\n"); break;
                case INTRINSECA_PRINT:  printf("Imprime una lista de valores.\n"); break;
                case INTRINSECA_PRINTWS:  printf("Imprime todas las variables del espacio de trabajo.\n"); break;
                case INTRINSECA_RESETWS:  printf("Resetea las variables del espacio de trabajo, como si se"
                                                 "se hubiese iniciado el programa por primera vez.\n"); break;
                case INTRINSECA_AYUDA:  printf("Devuelve ayuda sobre un valor pasado como argumento.\n"); break;
                case INTRINSECA_CARGAR: printf("Permite cargar un fichero en un string.\n"
                                               "Ejemplo: `load(\"math.mth\")`\n"); break;
                case INTRINSECA_EVAL: printf("Permite evaluar una cadena con código, como si se hubiese"
                                             "ejecutado directamente en la consola.\n"
                                             "Ejemplo: `eval(\"x=2+3\"); print(x)`\n"); break;
            }
            break;
    }
}

Valor cargar(Valor arg) {
    if (arg.tipo_valor != TIPO_STRING) {
        Error error = crear_error_op_incompatible("cargar ficheros", arg.tipo_valor);
        borrar_valor(&arg);
        return crear_valor_error(error, NULL);
    } else {
        FILE *fp = fopen(string_a_puntero(&arg.string), "r");
        if (!fp) {
            Error error = crear_error_archivo_no_encontrado(string_a_puntero(&arg.string));
            borrar_valor(&arg);
            return crear_valor_error(error, NULL);
        } else {
            fseek(fp, 0L, SEEK_END);
            long longitud = ftell(fp);
            rewind(fp);
            char *buffer = calloc(1, longitud+1);
            fread(buffer, longitud, 1, fp);
            fclose(fp);
            borrar_valor(&arg);

            String str = crear_string(buffer);
            return crear_valor_string(str, NULL);
        }
    }
}

Valor eval(Valor arg, TablaSimbolos *t) {
    if (arg.tipo_valor != TIPO_STRING) {
        Error error = crear_error_op_incompatible("evaluar", arg.tipo_valor);
        borrar_valor(&arg);
        return crear_valor_error(error, NULL);
    } else {
        Lexer lexer = crear_lexer_str(string_a_puntero(&arg.string));
        Evaluador evaluador = crear_evaluador(lexer);

        Valor result;
        while(evaluar_siguiente(&evaluador, t, &result)) {
            if (result.tipo_valor == TIPO_ERROR) {
                char* linea = obtener_linea(lexer, result.loc->first_line);
                imprimir_error(result.error, NULL, linea, result.loc);
                free(linea);
            }
            borrar_valor(&result);
        }

        borrar_valor(&arg);
        return result;
    }
}

Valor ejecutar_funcion_intrinseca(FuncionIntrinseca f, ListaValores args, TablaSimbolos *t) {
    Valor *vargs = (Valor*) args.valores;
    Valor result = crear_indefinido();

    switch (f) {
        case INTRINSECA_SUMA:
            comprobacion_n_args(2, "sumar");

            result = sumar(vargs[0], vargs[1]);
            break;
        case INTRINSECA_RESTA:
            comprobacion_n_args(2, "restar");

            result = restar(vargs[0], vargs[1]);
            break;
        case INTRINSECA_MULT:
            comprobacion_n_args(2, "multiplicar");

            result = mult(vargs[0], vargs[1]);
            break;
        case INTRINSECA_DIV:
            comprobacion_n_args(2, "dividir");

            result = dividir(vargs[0], vargs[1]);
            break;
        case INTRINSECA_MOD:
            comprobacion_n_args(2, "módulo");
            comprobacion_tipos(vargs[0], vargs[1], "módulo");

            result = modulo(vargs[0], vargs[1]);
            break;
        case INTRINSECA_EQ: {
            comprobacion_n_args(2, "igualdad");
            if (vargs[0].tipo_valor != vargs[1].tipo_valor) {
                result = crear_bool(0, NULL);
                break;
            }

            int resultado;
            if (comparar_valor(vargs[0], vargs[1], &resultado)) {
                result = crear_bool(resultado == 0, NULL);
            } else {
                result = crear_valor_error(crear_error_tipos_incompatibles("igualdad", vargs[0].tipo_valor, vargs[1].tipo_valor), NULL);
            }
            break;
        }
        case INTRINSECA_NEQ: {
            comprobacion_n_args(2, "desigualdad");
            if (vargs[0].tipo_valor != vargs[1].tipo_valor) {
                result = crear_bool(1, NULL);
                break;
            }

            int resultado;
            if (comparar_valor(vargs[0], vargs[1], &resultado)) {
                result = crear_bool(resultado != 0, NULL);
            } else {
                result = crear_valor_error(crear_error_tipos_incompatibles("desigualdad", vargs[0].tipo_valor, vargs[1].tipo_valor), NULL);
            }
            break;
        }
        case INTRINSECA_GE: {
            comprobacion_n_args(2, "mayor");
            int resultado;
            if (comparar_valor(vargs[0], vargs[1], &resultado)) {
                result = crear_bool(resultado > 0, NULL);
            } else {
                result = crear_valor_error(crear_error_tipos_incompatibles("mayor", vargs[0].tipo_valor, vargs[1].tipo_valor), NULL);
            }
            break;
        }
        case INTRINSECA_GEQ: {
            comprobacion_n_args(2, "mayor o igual");
            int resultado;
            if (comparar_valor(vargs[0], vargs[1], &resultado)) {
                result = crear_bool(resultado >= 0, NULL);
            } else {
                result = crear_valor_error(crear_error_tipos_incompatibles("mayor o igual", vargs[0].tipo_valor, vargs[1].tipo_valor), NULL);
            }
            break;
        }
        case INTRINSECA_LE: {
            comprobacion_n_args(2, "menor");
            int resultado;
            if (comparar_valor(vargs[0], vargs[1], &resultado)) {
                result = crear_bool(resultado < 0, NULL);
            } else {
                result = crear_valor_error(crear_error_tipos_incompatibles("menor", vargs[0].tipo_valor, vargs[1].tipo_valor), NULL);
            }
            break;
        }
        case INTRINSECA_LEQ: {
            comprobacion_n_args(2, "menor o igual");
            int resultado;
            if (comparar_valor(vargs[0], vargs[1], &resultado)) {
                result = crear_bool(resultado <= 0, NULL);
            } else {
                result = crear_valor_error(crear_error_tipos_incompatibles("menor o igual", vargs[0].tipo_valor, vargs[1].tipo_valor), NULL);
            }
            break;
        }
        case INTRINSECA_AND:
            comprobacion_n_args(2, "\"&&\"");
            comprobacion_tipos(vargs[0], vargs[1], "\"&&\"");

            result = and(vargs[0], vargs[1]);
            break;
        case INTRINSECA_OR:
            comprobacion_n_args(2, "\"||\"");
            comprobacion_tipos(vargs[0], vargs[1], "\"||\"");

            result = or(vargs[0], vargs[1]);
            break;
        case INTRINSECA_NOT:
            comprobacion_n_args(1, "negar");
            result = negacion(vargs[0]);
            break;
        case INTRINSECA_NEGAR:
            comprobacion_n_args(1, "negar");
            result = negar(vargs[0]);
            break;
        case INTRINSECA_PRINT:
            for (int i = 0; i < args.longitud; ++i)
                _imprimir_valor(vargs[i]);
            printf("\n");
            break;
        case INTRINSECA_PRINTWS:
            comprobacion_n_args(0, "imprimir espacio de trabajo");
            imprimir_simbolos(*t);
            break;
        case INTRINSECA_RESETWS:
            comprobacion_n_args(0, "resetear espacio de trabajo");
            borrar_tabla_simbolos(t);
            *t = crear_tabla_simbolos();
            inicializar_libreria_estandar(t);
            break;
        case INTRINSECA_AYUDA:
            if (args.longitud == 0) {
                printf("Esta es una función de ayuda. Pasa como parámetro un valor del que quieras saber información.\n"
                       "Ejemplo: `help(resetws)`\n\n"
                       "Para obtener ayuda sobre operadores, escríbelos entre paréntesis.\n"
                       "Ejemplo: `help((+))`\n");
            } else {
                ayuda(vargs[0]);
            }
            break;
        case INTRINSECA_CARGAR:
            comprobacion_n_args(1, "cargar");
            result = cargar(vargs[0]);
            break;
        case INTRINSECA_EVAL:
            comprobacion_n_args(1, "evaluar");

            result = eval(vargs[0], t);
            break;
    }

    free(args.valores);
    result.loc = args.loc;
    return result;
}