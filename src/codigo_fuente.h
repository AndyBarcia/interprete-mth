#ifndef PRACTICA3_CODIGO_FUENTE_H
#define PRACTICA3_CODIGO_FUENTE_H

#include <stdio.h>

/// Los distintos tipos de código fuente que
/// se aceptan en el analizador.
typedef enum {
    /// Un código fuente en forma de fichero.
    SRC_ARCHIVO,
    /// Un código fuente en forma de cadena de caracteres.
    SRC_STR,
} TipoCodigoFuente;

/// Un archivo con un descriptor y nombre dado.
typedef struct {
    /// Descriptor de archivo.
    FILE *fd;
    /// Nombre del archivo.
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

/**
 * Crea un código fuente a partir de una cadena de
 * caracteres reservada con malloc().
 * Su memoria se libera con free() cuando ya no sea
 * necesaria.
 * @param str cadena de caracteres.
 * @return
 */
CodigoFuente crear_codigo_fuente_str(char *str);

/**
 * Crea un código fuente a partir de una cadena de
 * caracteres, que es copiada con strcpy().
 * @param str cadena de caracteres.
 * @return
 */
CodigoFuente crear_codigo_fuente_str_cpy(char *str);

/**
 * Intenta crear un código fuente a partir de un
 * fichero con un path dado.
 * @param fichero el path del fichero.
 * @param codigo donde se guardará el objeto
 * de código fuente.
 * @return devuelve 1 si se consigue abrir el
 * fichero y 0 en caso contrario.
 */
int crear_codigo_fuente_archivo(char *fichero, CodigoFuente *codigo);

/**
 * Reduce el número de referencias dinámicas del
 * código fuente. Una vez desaparezcan todas las
 * referencias, se liberará la memoria asociada
 * al código fuente (se liberará la memoria dinámica,
 * o se cerrará el descriptor de archivo).
 * @param codigo
 */
void borrar_codigo_fuente(CodigoFuente *codigo);

/**
 * Realiza un clon de una localización en el código.
 * Esto aumenta el número de referencias dinámicas
 * del código fuente asociado.
 * @param loc
 * @return
 */
Localizacion clonar_loc(Localizacion loc);

/**
 * Borra una localización.
 * Esto reduce el número de referencias dinámicas
 * del código fuente asociado.
 * @param loc
 */
void borrar_loc(Localizacion *loc);

/**
 * Obtiene el nombre del fichero que está procesando
 * un lexer dado, o NULL si no está procesando un fichero.
 * @param lexer
 * @return
 */
char* obtener_nombre_fichero(CodigoFuente codigo);

/**
 * Obtiene una línea dada de un código fuente,
 * sin incluir el '\n' de final de línea.
 * La memoria devuelta se tiene que liberar con free()
 * @param codigo
 * @param linea
 * @return
 */
char* obtener_linea(CodigoFuente *codigo, int linea);

#endif //PRACTICA3_CODIGO_FUENTE_H
