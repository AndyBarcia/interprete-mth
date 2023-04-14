#include "codigo_fuente.h"
#include <stdlib.h>
#include <string.h>

CodigoFuente crear_codigo_fuente_str(char *str) {
    int* referencias = malloc(sizeof(int));
    *referencias = 1;

    return (CodigoFuente) {
        .tipo = SRC_STR,
        .referencias = referencias,
        .src.str = str,
    };
}

CodigoFuente crear_codigo_fuente_str_cpy(char *str) {
    return crear_codigo_fuente_str(strdup(str));
}

int crear_codigo_fuente_archivo(char *nombre, CodigoFuente *codigo) {
    FILE* fichero;
    if ((fichero = fopen(nombre, "r")) == NULL) {
        return 0;
    }

    int* referencias = malloc(sizeof(int));
    *referencias = 1;

    char* nombre_cp = strdup(nombre);

    *codigo = (CodigoFuente) {
            .tipo = SRC_ARCHIVO,
            .referencias = referencias,
            .src.archivo = (Archivo) {
                .fd = fichero,
                .nombre = nombre_cp
            },
    };

    return 1;
}

void borrar_codigo_fuente(CodigoFuente *codigo) {
    *codigo->referencias -= 1;
    if (*codigo->referencias <= 0) {
        switch (codigo->tipo) {
            case SRC_ARCHIVO:
                fclose(codigo->src.archivo.fd);
                codigo->src.archivo.fd = NULL;
                free(codigo->src.archivo.nombre);
                codigo->src.archivo.nombre = NULL;
                break;
            case SRC_STR:
                free(codigo->src.str);
                codigo->src.str = NULL;
                break;
        }
        free(codigo->referencias);
    }
    codigo->referencias = NULL;
}

Localizacion clonar_loc(Localizacion loc) {
    *loc.fuente.referencias += 1;
    return loc;
}

void borrar_loc(Localizacion *loc) {
    borrar_codigo_fuente(&loc->fuente);
}

char* obtener_nombre_fichero(CodigoFuente codigo) {
    switch (codigo.tipo) {
        case SRC_ARCHIVO:
            return codigo.src.archivo.nombre;
        case SRC_STR:
            return NULL;
    }
}

char* obtener_linea(CodigoFuente *codigo, int linea) {
    switch (codigo->tipo) {
        case SRC_ARCHIVO: {
            FILE* archivo = codigo->src.archivo.fd;

            // Guardar posición actual en el archivo y movernos al inicio
            long int pos = ftell(archivo);
            fseek(archivo, 0, SEEK_SET);

            char* buffer = NULL;
            size_t bufsize = 0;
            int current_line = 0;

            while (current_line < linea && getline(&buffer, &bufsize, archivo) != -1) {
                current_line++;
            }

            if (current_line < linea) {
                free(buffer);
                return NULL;
            }

            long len = strlen(buffer);
            if (buffer[len-1] == '\n')
                buffer[len-1] = '\0';

            // Volver a la posición original y terminar.
            fseek(archivo, pos, SEEK_SET);
            return buffer;
        }
        case SRC_STR: {
            char* actual = codigo->src.str;
            int current_line = 1;

            while (*actual && current_line < linea) {
                if (*actual == '\n') ++current_line;
                ++actual;
            }

            if (current_line < linea) {
                return NULL;
            }

            char* final_linea = actual+1;
            while (*final_linea && *final_linea != '\n')
                ++final_linea;

            long longitud = final_linea - actual;
            return strndup(actual, longitud);
        }
    }
}