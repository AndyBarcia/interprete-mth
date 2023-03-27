#include "definiciones.h"
#include <stdarg.h>
#include <stdio.h>

#include "analizador_sintactico.h"

ComponenteLexico crear_componente_lexico(TipoLexema tipo, String lexema) {
    return (ComponenteLexico) {
            .tipo = tipo,
            .lexema = lexema
    };
}

String crear_error_lexico(const char *formato, ...) {
    String error;
    // Cosas mágicas para pasar un número variable de argumentos.
    va_list args, args2;
    va_start(args, formato);
    va_copy(args2, args);
    // Calcular la longitud total que tendrá el string final.
    size_t longitud = vsnprintf(NULL, 0, formato, args);
    error = crear_string_n(longitud);
    // Escribir el resultado en el string creado.
    char *str = string_a_puntero(&error);
    vsnprintf(str, longitud + 1, formato, args2);
    va_end(args);
    va_end(args2);

    return error;
}

void borrar_componente_lexico(ComponenteLexico *componente_lexico) {
    // Solo borrar el string si no es una palabra clave y no es un identificador,
    // porque en ese caso el string está en la tabla hash y no lo podemos borrar.
    borrar_string(&componente_lexico->lexema);
    componente_lexico->tipo = -1;
}

char *obtener_lexema_str(ComponenteLexico *componenteLexico) {
    return string_a_puntero(&componenteLexico->lexema);
}

void imprimir_componente_lexico(ComponenteLexico componenteLexico) {
    printf("<%d, \"%s\">", componenteLexico.tipo, obtener_lexema_str(&componenteLexico));
}