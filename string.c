#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

String crear_string_vacio() {
    return (String) {
        .puntero = NULL,
        .referencias = NULL,
    };
}

String crear_string_n(unsigned long n) {
    if (n <= SMALL_STRING_PADDING) {
        return (String) {
                .puntero = NULL,
                .referencias = NULL,
        };
    } else {
        int* referencias = malloc(sizeof(int));
        *referencias = 1;

        return (String) {
                .puntero = malloc((n+1)*sizeof(char)),
                .referencias = referencias
        };
    }
}

String crear_string(char *str) {
    String string = crear_string_vacio();
    extender_string(&string, str);
    return string;
}

String crear_string_escapado(char *str) {
    long len = strlen(str);
    String s = crear_string_n(len);
    char* s_ptr = string_a_str(&s);

    long escrito = 0;
    for (long i = 0; i < len; ++i) {
        if (str[i] == '\\') {
            if (i+1 > len) {
                s_ptr[escrito++] = '\\';
                break;
            }
            switch (str[i+1]) {
                case '\"': s_ptr[escrito++] = '\"'; break;
                case '\'': s_ptr[escrito++] = '\''; break;
                case '\\': s_ptr[escrito++] = '\\'; break;
                case 'n': s_ptr[escrito++] = '\n'; break;
                case 'r': s_ptr[escrito++] = '\r'; break;
                case 't': s_ptr[escrito++] = '\t'; break;
                default: s_ptr[escrito++] = '\\'; break;
            }
            ++i;
        } else {
            s_ptr[escrito++] = str[i];
        }
    }
    s_ptr[escrito] = '\0';
    return s;
}

void extender_string(String *string, char *str) {
    extender_string_n(string, str, strlen(str));
}

void extender_string_n(String *string, char *str, unsigned long n) {
    unsigned long longitud_original = longitud_string(string);
    unsigned long longitud_nueva = n;
    unsigned long longitud = longitud_original + longitud_nueva;

    if (longitud <= SMALL_STRING_PADDING) {
        // La cadena sigue entrando en el padding.
        memcpy(string->padding + longitud_original, str, longitud_nueva);
        if (longitud < SMALL_STRING_PADDING) string->padding[longitud] = '\0';
        string->puntero = 0;
    } else if (!string->puntero) {
        // La cadena estaba en el padding, pero ya no entra.
        string->puntero = malloc((longitud + 1) * sizeof(char));
        memcpy(string->puntero, string->padding, longitud_original);
        memcpy(string->puntero + longitud_original, str, longitud_nueva);
        string->puntero[longitud] = '\0';
        string->referencias = malloc(sizeof(int));
        *string->referencias = 1;
    } else if (*string->referencias == 1) {
        // La cadena no estaba en el padding y esta es la única referencia
        // que hay al string (se puede modificar sin problemas).
        string->puntero = realloc(string->puntero, (longitud+1)*sizeof(char));
        memcpy(string->puntero + longitud_original, str, longitud_nueva);
        string->puntero[longitud] = '\0';
    } else {
        // La cadena no estaba en el padding, y hay múltiples referencias
        // al string (tenemos que hacer una copia para modificarla).
        char* clon = malloc((longitud + 1) * sizeof(char));
        memcpy(clon, string->puntero, longitud_original);
        memcpy(clon + longitud_original, str, longitud_nueva);
        clon[longitud] = '\0';
        string->puntero = clon;

        --(*string->referencias);
        string->referencias = malloc(sizeof(int));
        *string->referencias = 1;
    }
}

String clonar_string(String string) {
    if (string.puntero)
        ++(*string.referencias);
    return string;
}

void borrar_string(String *string) {
    if (string->puntero) {
        --(*string->referencias);
        if (*string->referencias <= 0) {
            free(string->puntero);
            free(string->referencias);
        }
        string->puntero = NULL;
        string->referencias = NULL;
    } else {
        string->padding[0] = '\0';
    }
}

char *string_a_str(String *string) {
    return string->puntero ? string->puntero : string->padding;
}

unsigned long longitud_string(String *string) {
    return strlen(string_a_str(string));
}