#include "std.h"
#include "ast/ast.h"
#include "math.h"

void inicializar_libreria_estandar(TablaSimbolos *t) {
    asignar_valor_tabla(t, crear_string("verdadero"), crear_bool(1, NULL), ASIGNACION_INMUTABLE);
    asignar_valor_tabla(t, crear_string("falso"), crear_bool(0, NULL), ASIGNACION_INMUTABLE);
    asignar_valor_tabla(t, crear_string("pi"), crear_decimal(M_PI, NULL), ASIGNACION_INMUTABLE);
    asignar_valor_tabla(t, crear_string("e"), crear_decimal(M_E, NULL), ASIGNACION_INMUTABLE);
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
    }

    free(args.valores);
    result.loc = args.loc;
    return result;
}