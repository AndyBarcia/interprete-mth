#ifndef PRACTICA3_CODIGO_FUENTE_H
#define PRACTICA3_CODIGO_FUENTE_H

#include <stdio.h>

/// Los distintos tipos de código fuente que
/// se aceptan en el analizador.
typedef enum {
    SRC_ARCHIVO,
    SRC_STR,
} TipoCodigoFuente;

/// Un archivo con un descriptor y nombre dado.
typedef struct {
    FILE *fd;
    char* nombre;
} Archivo;

/// Una representación abstracta de un código
/// fuente, ya sea en forma de un fichero
/// o una cadena de caracteres en memoria.
/// Se lleva una cuenta de número de referencias
/// activas para que se cierre automáticamente
/// el archivo cuando sea necesario.
typedef struct {
    TipoCodigoFuente tipo;
    int *referencias;
    union {
        Archivo archivo;
        char* str;
    } src;
} CodigoFuente;

/// Localización de un componente léxico en
/// el código fuente que se esté procesando.
typedef struct {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
    CodigoFuente fuente;
}  Localizacion;

CodigoFuente crear_codigo_fuente_str(char *str);
CodigoFuente crear_codigo_fuente_str_cpy(char *str);
int crear_codigo_fuente_archivo(char *nombre, CodigoFuente *codigo);
void borrar_codigo_fuente(CodigoFuente *codigo);

Localizacion clonar_loc(Localizacion loc);
void borrar_loc(Localizacion *loc);

/**
 * Obtiene el nombre del fichero que está procesando
 * un lexer dado, o NULL si no está procesando un fichero.
 * @param lexer
 * @return
 */
char* obtener_nombre_fichero(CodigoFuente codigo);

char* obtener_linea(CodigoFuente *codigo, int linea);

#endif //PRACTICA3_CODIGO_FUENTE_H
