#include "error.h"
#include <stdarg.h>

Error crear_error(const char *formato, ...) {
    String mensaje;
    // Cosas mágicas para pasar un número variable de argumentos.
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

void borrar_error(Error *error) {
    borrar_string(&error->mensaje);
}

void imprimir_error(Error error, char* nombre, char* linea, Localizacion *loc) {
    printf("error: %s\n", string_a_puntero(&error.mensaje));
    if (nombre)
        printf("  -> %s\n", nombre);
    if (loc) {
        printf("   |\n");
        printf("%*d | %s\n", 2, loc->first_line+1, linea);
        printf("   | ");

        for (int i = 0; i < loc->first_column-1; ++i)
            printf(" ");
        int width = (loc->last_column - loc->first_column)+1;
        for (int i = 0; i < width; ++i)
            printf("^");
        printf("\n");
    }
}