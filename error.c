#include "error.h"
#include "ast/valor.h"
#include <stdarg.h>

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

Error crear_error_numero_argumentos(int esperado, int actual) {
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
            return crear_error("Se esperaba %d argumentos, pero se recibió sólo uno.", esperado);
        } else {
            return crear_error("Se esperaban %d argumentos, pero se recibieron %d.", esperado, actual);
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

void borrar_error(Error *error) {
    borrar_string(&error->mensaje);
}

void imprimir_error(Error error, char* nombre, char* linea, Localizacion *loc) {
    printf("error: %s\n", string_a_puntero(&error.mensaje));
    if (nombre)
        printf("  -> %s\n", nombre);
    if (loc) {
        printf("   |\n");
        printf("%*d | %s\n", 2, loc->first_line, linea);
        printf("   | ");

        for (int i = 0; i < loc->first_column-1; ++i)
            printf(" ");
        int width = (loc->last_column - loc->first_column);
        for (int i = 0; i < width; ++i)
            printf("^");
        printf("\n");
    }
}