#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *buffer_strings = NULL;
unsigned long capacidad_buffer = 0;
char *siguiente_posicion_libre = NULL;

char **buffer_indices = NULL;
unsigned long capacidad_indices = 0;
PunteroString siguienteIndiceLibre = 1;

void crear_buffer_strings() {
    capacidad_buffer = 1024;
    buffer_strings = malloc(capacidad_buffer * sizeof(char));
    buffer_strings[0] = '\0';
    siguiente_posicion_libre = &buffer_strings[0];

    capacidad_indices = 256;
    buffer_indices = malloc(capacidad_indices * sizeof(char *));
    buffer_indices[0] = NULL;
    siguienteIndiceLibre = 1;
}

void crecer_buffer_strings() {
    // Duplicar la capacidad para almacenar strings-
    capacidad_buffer *= 2;
    char *antiguo_buffer_strings = buffer_strings;
    buffer_strings = realloc(buffer_strings, capacidad_buffer * sizeof(char));
    // Si realloc nos movió a otra posición de memoria, actualizar los índices.
    if (antiguo_buffer_strings != buffer_strings) {
        for (int i = 0; i < siguienteIndiceLibre; ++i) {
            unsigned long offset = buffer_indices[i] - antiguo_buffer_strings;
            buffer_indices[i] = buffer_strings + offset;
        }
        unsigned long offset = siguiente_posicion_libre - antiguo_buffer_strings;
        siguiente_posicion_libre = buffer_strings + offset;
    }
}

void crecer_buffer_indices() {
    // Actualizar la capacidad de índices.
    capacidad_indices *= 2;
    buffer_indices = realloc(buffer_indices, capacidad_indices * sizeof(char *));
}

PunteroString generar_nuevo_indice(unsigned long longitud) {
    if (siguienteIndiceLibre >= capacidad_indices)
        crecer_buffer_indices();
    if ((siguiente_posicion_libre - buffer_strings) + longitud + 1 >= capacidad_buffer)
        crecer_buffer_strings();
    buffer_indices[siguienteIndiceLibre] = siguiente_posicion_libre;
    siguiente_posicion_libre += longitud + 1;

    return siguienteIndiceLibre++;
}

void borrar_buffer_strings() {
    free(buffer_strings);
    buffer_strings = NULL;
    capacidad_buffer = 0;
    siguiente_posicion_libre = NULL;

    free(buffer_indices);
    buffer_indices = NULL;
    capacidad_indices = 0;
    siguienteIndiceLibre = 0;
}

String crear_string_vacio() {
    String string;
    string.padding[0] = '\0';
    string.puntero = 0;
    return string;
}

String crear_string(char *str) {
    String string = crear_string_vacio();
    extender_string(&string, str);
    return string;
}

String crear_string_escapado(char *str) {
    long len = strlen(str);
    String s = crear_string_n(len);
    char* s_ptr = string_a_puntero(&s);

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
        // Asegurarse de que entra toda la cadena en el buffer.
        if ((siguiente_posicion_libre - buffer_strings) + longitud + 1 >= capacidad_buffer)
            crecer_buffer_strings();
        PunteroString indice = generar_nuevo_indice(longitud);
        memcpy(buffer_indices[indice], string->padding, longitud_original);
        memcpy(buffer_indices[indice] + longitud_original, str, longitud_nueva);
        buffer_indices[indice][longitud] = '\0';
        string->puntero = indice;
        string->padding[0] = '\0';
    } else {
        // La cadena no estaba en el padding.
        // Asegurarse que la sección añadida entra en el buffer.
        if ((siguiente_posicion_libre - buffer_strings) + longitud_nueva + 1 >= capacidad_buffer)
            crecer_buffer_strings();
        // Comprobar el string que estamos extendiendo está al final del buffer.
        if (siguienteIndiceLibre - 1 == string->puntero) {
            memcpy(buffer_indices[string->puntero] + longitud_original, str, longitud_nueva);
            buffer_indices[string->puntero][longitud] = '\0';
            siguiente_posicion_libre += longitud_nueva;
        } else {
            // Para extender este string tendría que mover strings a la derecha
            // para hacer espacio, pero como esta situación no se da en este
            // programa, no me molesto en implementarla.
            printf("Extensión: Operación no soportada aún.\n");
            exit(1);
        }
    }
}

String crear_string_n(unsigned long n) {
    if (n <= SMALL_STRING_PADDING) {
        return (String) {
                .puntero = 0,
        };
    } else {
        return (String) {
                .puntero = generar_nuevo_indice(n)
        };
    }
}

String clonar_string(String string) {
    return crear_string(string_a_puntero(&string));
}

void borrar_string(String *string) {
    if (string->puntero) {
        // Comprobar si el string que estamos borrando es la última en el buffer.
        if (siguienteIndiceLibre - 1 == string->puntero) {
            siguiente_posicion_libre = buffer_indices[string->puntero];
            --siguienteIndiceLibre;
        } else {
            // Si no, tendríamos que mover strings a la izquierda para aprovechar
            // espacio, pero como en este programa esto casi no afecta, pues
            // no me molesto en implementarlo.
            // printf("Borrado: Operación no soportada aún.\n");
        }
        string->puntero = 0;
        string->padding[0] = '\0';
    } else {
        string->padding[0] = '\0';
    }
}

char *string_a_puntero(String *string) {
    return string->puntero ? buffer_indices[string->puntero] : string->padding;
}

unsigned long longitud_string(String *string) {
    return strlen(string_a_puntero(string));
}