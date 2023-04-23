#include "std.h"
#include "ast/ast.h"
#include "evaluador.h"
#include "bibliotecas/biblioteca_dinamica.h"

void cargar_intrinsecas(TablaSimbolos *t) {
    asignar_valor_tabla(t, crear_string("print"), crear_funcion_intrinseca(INTRINSECA_PRINT, NULL), ASIGNACION_INMUTABLE);
    asignar_valor_tabla(t, crear_string("printws"), crear_funcion_intrinseca(INTRINSECA_PRINTWS, NULL), ASIGNACION_INMUTABLE);
    asignar_valor_tabla(t, crear_string("resetws"), crear_funcion_intrinseca(INTRINSECA_RESETWS, NULL), ASIGNACION_INMUTABLE);
    asignar_valor_tabla(t, crear_string("help"), crear_funcion_intrinseca(INTRINSECA_AYUDA, NULL), ASIGNACION_INMUTABLE);
    asignar_valor_tabla(t, crear_string("load"), crear_funcion_intrinseca(INTRINSECA_CARGAR, NULL), ASIGNACION_INMUTABLE);
    asignar_valor_tabla(t, crear_string("eval"), crear_funcion_intrinseca(INTRINSECA_EVAL, NULL), ASIGNACION_INMUTABLE);
    asignar_valor_tabla(t, crear_string("exit"), crear_funcion_intrinseca(INTRINSECA_EXIT, NULL), ASIGNACION_INMUTABLE);
    asignar_valor_tabla(t, crear_string("callforeign"), crear_funcion_intrinseca(INTRINSECA_CALLFOREIGN, NULL), ASIGNACION_INMUTABLE);
    asignar_valor_tabla(t, crear_string("int"), crear_funcion_intrinseca(INTRINSECA_CAST_ENTERO, NULL), ASIGNACION_INMUTABLE);
    asignar_valor_tabla(t, crear_string("decimal"), crear_funcion_intrinseca(INTRINSECA_CAST_DECIMAL, NULL), ASIGNACION_INMUTABLE);
}

/// Macro de ayuda para comprobar que se han pasado el rango correcto
/// de número de argumentos. Si el número de argumentos es menor que
/// n_args o mayor que m_args se devuelve un error.
/// Si m_args es -1, viene implícito que se acepta un número arbitrariamente
/// alto de argumentos.
#define comprobacion_n_m_args(n_args, m_args, op) \
    if (args.longitud < n_args || (m_args != -1 && args.longitud > m_args)) { \
        Valor v = crear_valor_error(crear_error_numero_argumentos(n_args, m_args, args.longitud), args.loc); \
        borrar_lista_valores(&args); \
        return v; \
    }

/// Lo mismo que la anterior pero para funciones que aceptan un número fijo de argumentos.
#define comprobacion_n_args(n_args, op) comprobacion_n_m_args(n_args, n_args, op)

/// Facilidad para comparar argumentos con operaciones '==', '>', '>=', etc
#define comparar_args(a, b, comp) \
    int resultado;                \
    if (comparar_valor(a, b, &resultado)) { \
        result = crear_bool(resultado comp, NULL); \
    } else if (a.tipo_valor != b.tipo_valor) { \
        result = crear_bool(0, NULL); \
    } else { \
        result = crear_valor_error(crear_error_tipos_incompatibles("comparar", a.tipo_valor, b.tipo_valor), NULL); \
    } \
    borrar_valor(&a); \
    borrar_valor(&b); \

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
                    String x = clonar_string(a.string);
                    borrar_valor(&a);

                    extender_string(&x, string_a_str(&b.string));
                    borrar_valor(&b);

                    return crear_valor_string(x, NULL);
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
                    String x = clonar_string(b.string);
                    for (int i = 1; i < a.entero; ++i)
                        extender_string(&x, string_a_str(&b.string));
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
            switch (b.tipo_valor) {
                case TIPO_ENTERO:
                    result = crear_entero(a.entero % b.entero, NULL);
                    break;
                default:
                    result = crear_valor_error(crear_error_tipos_incompatibles("módulo", a.tipo_valor, b.tipo_valor), NULL);
                    break;
            }
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
            switch (b.tipo_valor) {
                case TIPO_BOOL:
                    result = crear_bool(a.bool && b.bool, NULL);
                    break;
                default:
                    result = crear_valor_error(crear_error_tipos_incompatibles("\"&&\"", a.tipo_valor, b.tipo_valor), NULL);
                    break;
            }
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
            switch (b.tipo_valor) {
                case TIPO_BOOL:
                    result = crear_bool(a.bool || b.bool, NULL);
                    break;
                default:
                    result = crear_valor_error(crear_error_tipos_incompatibles("\"||\"", a.tipo_valor, b.tipo_valor), NULL);
                    break;
            }
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
        case TIPO_CONTROL_FLUJO:
            // No debería de pasar nunca.
            break;
        case TIPO_INDEFINIDO:
            printf("Este es un tipo nulo. No deberías de poder tener acceso a él :D");
            break;
        case TIPO_UNIDAD:
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
                case INTRINSECA_SUMA: printf("Permite sumar dos números, o concatenar dos cadenas de caracteres.\n"); break;
                case INTRINSECA_RESTA: printf("Permite restar dos números.\n"); break;
                case INTRINSECA_MULT: printf("Permite multiplicar dos números, o concatenar repetidamente "
                                             "un string a sí mismo.\n"); break;
                case INTRINSECA_DIV: printf("Permite dividir de forma exacta dos números, siempre devolviendo un "
                                            "decimal.\nPara una versión para enters, utiliza el operador (//)."); break;
                case INTRINSECA_MOD: printf("Permite utilizar aritmética modular.\n"); break;
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
                                               "Ejemplo: `load(\"prelude.mth\")`\n"); break;
                case INTRINSECA_EVAL: printf("Permite evaluar una cadena con código, como si se hubiese"
                                             "ejecutado directamente en la consola.\n"
                                             "Ejemplo: `eval(\"x=2+3\"); print(x)`\n"); break;
                case INTRINSECA_EXIT: printf("Termina la ejecución del programa.\n"); break;
                case INTRINSECA_CALLFOREIGN: printf("Permite llamar a una función foránea de una biblioteca dinámica"
                                                    "de C. Los tipos de los argumentos de la función se deciden"
                                                    "de forma dinámica en base a los argumentos pasados, mientras que"
                                                    "el tipo de retorno se indica explícitamente.\nPor ejemplo, para"
                                                    "llamar a una función\n\n\t\t double sumar(double a, double b);\n\n"
                                                    "Se haría lo siguiente:\n\n\t\t"
                                                    "import foreign \"biblioteca.so\" as bib\n\t\t"
                                                    "callforeign(bib.sumar, \"double\", 2.0, 3.0)\n\n"
                                                    "Es importante que los argumentos pasados sean del tipo correcto,"
                                                    "pues si no se pueden producir errores no predecibles.\n"); break;
                case INTRINSECA_CAST_ENTERO:
                case INTRINSECA_CAST_DECIMAL:
                    printf("Realiza un cast de un tipo a otro, o devuelve un error en caso de que el cast no se"
                           "pueda realizar.\n");
                    break;
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
        FILE *fp = fopen(string_a_str(&arg.string), "r");
        if (!fp) {
            Error error = crear_error_archivo_no_encontrado(string_a_str(&arg.string));
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
            free(buffer);
            return crear_valor_string(str, NULL);
        }
    }
}

// Implementación de la función `eval`, que permite evaluar un string creado
// por el usuario como si se hubiese ejecutado en la terminal.
// Se aplican algunas restricciones para que la ejecución de código arbitrario
// sea seguro:
//  * Se ejecuta en su propio nivel de la tabla de símbolos, por lo que no puede
//    modificar variables definidas anteriormente.
//  * Se capturan valores de control de flujo creados, por lo que no
//    se puede terminar la ejecución de la terminal con `eval`, ni tampoco
//    salir prematuramente de una función, ni terminar un bucle.
Valor eval(Valor arg, Evaluador *evaluador) {
    if (arg.tipo_valor != TIPO_STRING) {
        // Si el único argumento no es un string, reportar un error.
        Error error = crear_error_op_incompatible("evaluar", arg.tipo_valor);
        borrar_valor(&arg);
        return crear_valor_error(error, NULL);
    } else {
        // Si no, crear un evaluador en contexto de EVAL.
        CodigoFuente src = crear_codigo_fuente_str_cpy(string_a_str(&arg.string));
        Lexer lexer = crear_lexer(src);
        Parser parser = crear_parser(lexer, CONTEXTO_EVAL);

        // Aumentar el nivel de la tabla de símbolos y establecer
        // una barrera, para que no se puedan modificar variables
        // externas desde dentro del `eval`.
        aumentar_nivel_tabla_simbolos(evaluador->tabla_simbolos);
        establecer_barrera_tabla_simbolos(evaluador->tabla_simbolos);

        Valor result = crear_valor_unidad(NULL);
        Expresion exp;
        while (siguiente_expresion(&parser, &exp)) {
            Valor v = evaluar_expresion(evaluador, &exp);
            if (result.tipo_valor == TIPO_ERROR) imprimir_valor(v);
            borrar_valor(&result);
            borrar_expresion(&exp);
            result = v;
        }

        reducir_nivel_tabla_simbolos(evaluador->tabla_simbolos);
        borrar_parser(&parser);
        borrar_valor(&arg);

        // Importante: si el resultado es un tipo de control
        // de flujo, capturarlo para que no se propage fuera
        // de la llamada a `eval`.
        if (result.tipo_valor == TIPO_CONTROL_FLUJO) {
            if (result.control_flujo.valor) {
                Valor r = *(Valor*) result.control_flujo.valor;
                borrar_valor(&result);
                result = r;
            } else {
                Valor r = crear_valor_unidad(result.loc);
                borrar_valor(&result);
                result = r;
            }
        }

        return result;
    }
}

void _imprimir_valor(Valor valor);

void _imprimir_entrada_tabla(EntradaTablaHash entrada) {
    // No imprimir las funciones intrínsecas para evitar saturar la consola.
    if (entrada.valor.tipo_valor != TIPO_FUNCION_INTRINSECA) {
        printf("%s := ", string_a_str(&entrada.clave));
        imprimir_valor(entrada.valor);
    }
}

void printws(TablaSimbolos *t) {
    for (int i = 0; i <= t->nivel; ++i) {
        iterar_tabla_hash(t->tablas[i], _imprimir_entrada_tabla);
    }
}

Valor callforeign(Valor f, Valor tipo_retorno, Valor* vargs, int nargs) {
    if (f.tipo_valor != TIPO_FUNCION_FORANEA) {
        Error error = crear_error_op_incompatible("utilizar como función foránea", f.tipo_valor);
        Valor result = crear_valor_error(error, f.loc);
        borrar_valor(&f);
        borrar_valor(&tipo_retorno);
        for (int i = 0; i < nargs; ++i)
            borrar_valor(vargs+i);
        return result;
    }
    if (tipo_retorno.tipo_valor != TIPO_STRING) {
        Error error = crear_error_op_incompatible("utilizar como tipo de retorno", tipo_retorno.tipo_valor);
        Valor result = crear_valor_error(error, tipo_retorno.loc);
        borrar_valor(&f);
        borrar_valor(&tipo_retorno);
        for (int i = 0; i < nargs; ++i)
            borrar_valor(vargs+i);
        return result;
    }

    char* str_retorno = string_a_str(&tipo_retorno.string);
    TipoForaneo tipo_r;
    if (strcmp(str_retorno, "int") == 0) {
        tipo_r = TIPO_C_INT;
    } else if (strcmp(str_retorno, "float") == 0) {
        tipo_r = TIPO_C_FLOAT;
    } else if (strcmp(str_retorno, "double") == 0) {
        tipo_r = TIPO_C_DOUBLE;
    } else if (strcmp(str_retorno, "void") == 0) {
        tipo_r = TIPO_C_VOID;
    } else {
        Error error = crear_error("No se reconoce \"%s\" como un tipo de retorno correcto para una función foránea.", str_retorno);
        borrar_valor(&f);
        borrar_valor(&tipo_retorno);
        for (int i = 0; i < nargs; ++i)
            borrar_valor(vargs+i);
        return crear_valor_error(error, tipo_retorno.loc);
    }

    Valor resultado = llamar_funcion_foranea(f.funcion_foranea, tipo_r, vargs, nargs);

    borrar_valor(&f);
    borrar_valor(&tipo_retorno);
    for (int i = 0; i < nargs; ++i)
        borrar_valor(vargs+i);
    return resultado;
}

Valor casting(Valor v, TipoValor objetivo) {
    if (v.tipo_valor == objetivo)
        return v;

    switch (objetivo) {
        case TIPO_ENTERO:
            switch (v.tipo_valor) {
                case TIPO_DECIMAL: {
                    Valor c = crear_entero((int) v.decimal, NULL);
                    borrar_valor(&v);
                    return c;
                }
                default: break;
            }
            break;
        case TIPO_DECIMAL:
            switch (v.tipo_valor) {
                case TIPO_ENTERO: {
                    Valor c = crear_decimal((Decimal) v.entero, NULL);
                    borrar_valor(&v);
                    return c;
                }
                default: break;
            }
            break;
        default: break;
    }

    Error error = crear_error_casting(v.tipo_valor, objetivo);
    Valor retorno = crear_valor_error(error, v.loc);
    borrar_valor(&v);
    return retorno;
}

Valor ejecutar_funcion_intrinseca(FuncionIntrinseca f, ListaValores args, Evaluador *evaluador) {
    Valor *vargs = (Valor*) args.valores;
    Valor result = crear_valor_unidad(NULL);

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
            result = modulo(vargs[0], vargs[1]);
            break;
        case INTRINSECA_EQ: {
            comprobacion_n_args(2, "igualdad");
            comparar_args(vargs[0], vargs[1], == 0);
            break;
        }
        case INTRINSECA_NEQ: {
            comprobacion_n_args(2, "desigualdad");
            comparar_args(vargs[0], vargs[1], != 0);
            break;
        }
        case INTRINSECA_GE: {
            comprobacion_n_args(2, "mayor");
            comparar_args(vargs[0], vargs[1], > 0);
            break;
        }
        case INTRINSECA_GEQ: {
            comprobacion_n_args(2, "mayor o igual");
            comparar_args(vargs[0], vargs[1], >= 0);
            break;
        }
        case INTRINSECA_LE: {
            comprobacion_n_args(2, "menor");
            comparar_args(vargs[0], vargs[1], < 0);
            break;
        }
        case INTRINSECA_LEQ: {
            comprobacion_n_args(2, "menor o igual");
            comparar_args(vargs[0], vargs[1], <= 0);
            break;
        }
        case INTRINSECA_AND:
            comprobacion_n_args(2, "\"&&\"");
            result = and(vargs[0], vargs[1]);
            break;
        case INTRINSECA_OR:
            comprobacion_n_args(2, "\"||\"");
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
            for (int i = 0; i < args.longitud; ++i) {
                _imprimir_valor(vargs[i]);
                borrar_valor(&vargs[i]);
            }
            printf("\n");
            break;
        case INTRINSECA_PRINTWS:
            comprobacion_n_args(0, "imprimir espacio de trabajo");
            printws(evaluador->tabla_simbolos);
            break;
        case INTRINSECA_RESETWS:
            comprobacion_n_args(0, "resetear espacio de trabajo");
            borrar_tabla_simbolos(evaluador->tabla_simbolos);
            *evaluador->tabla_simbolos = crear_tabla_simbolos();
            cargar_intrinsecas(evaluador->tabla_simbolos);
            break;
        case INTRINSECA_AYUDA:
            comprobacion_n_m_args(0, 1, "pedir ayuda");
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
            result = eval(vargs[0], evaluador);
            break;
        case INTRINSECA_EXIT:
            comprobacion_n_m_args(0, 1, "salir");
            if (args.longitud == 1)
                result = crear_valor_control_flujo(CTR_FLUJO_EXIT, &vargs[0], NULL);
            else
                result = crear_valor_control_flujo(CTR_FLUJO_EXIT, NULL, NULL);
            break;
        case INTRINSECA_CALLFOREIGN:
            comprobacion_n_m_args(3, -1, "llamar funciones foráneas");
            result = callforeign(vargs[0], vargs[1], vargs+2, args.longitud-2);
            break;
        case INTRINSECA_CAST_ENTERO:
            comprobacion_n_args(1, "casting");
            result = casting(vargs[0], TIPO_ENTERO);
            break;
        case INTRINSECA_CAST_DECIMAL:
            comprobacion_n_args(1, "casting");
            result = casting(vargs[0], TIPO_DECIMAL);
            break;
    }

    free(args.valores);
    if (!result.loc) {
        result.loc = args.loc;
    } else if (args.loc) {
        borrar_loc(args.loc);
        free(args.loc);
    }
    return result;
}
