#include "error.h"
#include "ast/valor.h"
#include <stdarg.h>
#include <stdlib.h>

Error crear_error(const char *formato, ...) {
    String mensaje;
    // Cosas mágicas para pasar un número variable de nombres_args.
    va_list args, args2;
    va_start(args, formato);
    va_copy(args2, args);
    // Calcular la longitud total que tendrá el string final.
    size_t longitud = vsnprintf(NULL, 0, formato, args);
    mensaje = crear_string_n(longitud);
    // Escribir el resultado en el string creado.
    char *str = string_a_puntero(&mensaje);
    vsnprintf(str, longitud + 1, formato, args2);
    va_end(args);
    va_end(args2);

    return (Error) { mensaje };
}

Error crear_error_variable_no_definida(char* nombre) {
    return crear_error("\"%s\" es una variable no definida.", nombre);
}

Error crear_error_reasignando_inmutable(char* nombre) {
    return crear_error("Intentando reasignar variable inmutable \"%s\"", nombre);
}

Error crear_error_acceso_incorrecto(int tipo_base, int tipo_indice) {
    return crear_error("No se pueden indexar valores de tipo %s con un %s", tipo_valor_a_str(tipo_base), tipo_valor_a_str(tipo_indice));
}

Error crear_error_numero_argumentos(int esperado_n, int esperado_m, int actual) {
    if (esperado_n == esperado_m) {
        int esperado = esperado_n;
        if (esperado == 0) {
            if (actual == 1) {
                return crear_error("No se esperaban argumentos, pero se recibió uno.");
            } else {
                return crear_error("No se esperaban argumentos, pero se recibieron %d.", actual);
            }
        } else if (esperado == 1) {
            if (actual == 0) {
                return crear_error("Se esperaba un argumento, pero no se recibió ninguno.");
            } else {
                return crear_error("Se esperaba un argumento, pero se recibieron %d.", actual);
            }
        } else {
            if (actual == 0) {
                return crear_error("Se esperaban %d argumentos, pero no se recibió ninguno.", esperado);
            } else if (actual == 1) {
                return crear_error("Se esperaban %d argumentos, pero se recibió sólo uno.", esperado);
            } else {
                return crear_error("Se esperaban %d argumentos, pero se recibieron %d.", esperado, actual);
            }
        }
    } else if (esperado_m == -1) {
        if (actual == 0) {
            return crear_error("Se esperaban %d o más argumentos, pero no se recibió ninguno.", esperado_n);
        } else if (actual == 1) {
            return crear_error("Se esperaban %d o más argumento, pero se recibió sólo uno.", esperado_n);
        } else {
            return crear_error("Se esperaban %d o más argumento, pero se recibieron %d.", esperado_n, actual);
        }
    } else {
        if (actual == 0) {
            return crear_error("Se esperaban entre %d y %d argumentos, pero no se recibió ninguno.", esperado_n, esperado_m);
        } else if (actual == 1) {
            return crear_error("Se esperaban entre %d y %d argumentos, pero se recibió sólo uno.", esperado_n, esperado_m);
        } else {
            return crear_error("Se esperaban entre %d y %d argumentos, pero se recibieron %d.", esperado_n, esperado_m, actual);
        }
    }
}

Error crear_error_tipos_incompatibles(char* op, int tipoA, int tipoB) {
    return crear_error("No se pueden %s valores de tipo %s y %s.", op, tipo_valor_a_str(tipoA), tipo_valor_a_str(tipoB));
}

Error crear_error_op_incompatible(char* op, int tipo) {
    return crear_error("No se pueden %s valores de tipo %s.", op, tipo_valor_a_str(tipo));
}

Error crear_error_dividir_entre_cero() {
    return crear_error("No se puede dividir entre cero.");
}

Error crear_error_archivo_no_encontrado(char* fichero) {
    return crear_error("No se ha podido cargar el fichero \"%s\".", fichero);
}

Error crear_error_contexto_incorrecto(char* tipo, char* contexto) {
    return crear_error("No se puede user %s desde %s.", tipo, contexto);
}

Error crear_error_casting(int tipoActual, int tipoObjetivo) {
    return crear_error("No se puede hacer un cast de %s a %s.", tipo_valor_a_str(tipoActual), tipo_valor_a_str(tipoObjetivo));
}

void borrar_error(Error *error) {
    borrar_string(&error->mensaje);
}

void imprimir_error(Error error, Localizacion *loc) {
    printf("error: %s\n", string_a_puntero(&error.mensaje));
    if (loc) {
        if (loc->fuente.tipo == SRC_ARCHIVO)
            printf("  -> %s\n", loc->fuente.src.archivo.nombre);
        printf("   |\n");

        char* linea = obtener_linea(&loc->fuente, loc->first_line);
        printf("%*d | %s\n", 2, loc->first_line, linea);
        free(linea);

        printf("   | ");

        for (int i = 0; i < loc->first_column-1; ++i)
            printf(" ");
        int width = (loc->last_column - loc->first_column);
        for (int i = 0; i < width; ++i)
            printf("^");
        printf("\n");
    }
}