#ifndef LEXER_ANALIZADOR_LEXICO_H
#define LEXER_ANALIZADOR_LEXICO_H

#include <stdio.h>

/// Localización de un componente léxico en
/// el código fuente que se esté procesando.
typedef struct {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
}  Localizacion;

/// Un determinado analizador léxico que es
/// abstracto respecto al origen del código
/// fuente; ya sea un fichero, stdin, o una
/// cadena de caracteres.
typedef struct {
    // El escáner de bison
    void* scanner;
    // Buffer que se utiliza si
    // estamos procesando un char*
    // en vez de un archivo.
    void* str_buffer;
    // String que se está procesando,
    // si es que se está procesando
    // una cadena y no un fichero.
    char* str;
    // Nombre del fichero que se
    // está analizando, o NULL en
    // caso de estar procesando
    // un archivo o un string.
    char* nombre_fichero;
    // Posición actual del analizador.
    Localizacion loc;
} Lexer;

/**
 * Crea un analizador léxico sobre un determinado archivo.
 * @param lexer lugar donde se creará el analizador
 * @param archivo fichero a procesar.
 * @return 1 si se abrió el archivo y 0 en caso contrario.
 */
int crear_lexer_archivo(Lexer *lexer, char *archivo);

/**
 * Crea un analizador léxico sobre una cadena de caracteres.
 * @param str cadena a analizar.
 * @return el analizador léxico
 */
Lexer crear_lexer_str(char *str);

/**
 * Libera la memoria de un analizador léxico dado.
 * @param lexer
 */
void borrar_analizador_lexico(Lexer lexer);

/**
 * Obtiene una línea del código fuente de un lexer dado.
 * @param lexer el analizador léxico.
 * @param loc la localización de la línea.
 * @return la línea del código fuente
 */
char* obtener_linea(Lexer lexer, int linea);

/**
 * Obtiene el nombre del fichero que está procesando
 * un lexer dado, o NULL si no está procesando un fichero.
 * @param lexer
 * @return
 */
char* obtener_nombre_fichero(Lexer lexer);

/**
 * Obtiene la localización actual de un analizador
 * léxico dado.
 * @param lexer
 * @return
 */
Localizacion obtener_localizacion(Lexer lexer);

/**
 * Devuelve el siguiente componente léxico de un analizador
 * léxico dado.
 * @param lexer analizador léxico a utilizar.
 * @param token tipo opaco en el que se guardará el componente
 * léxico devuelto.
 * @return tipo de token devuelto.
 */
int siguiente_componente_lexico(Lexer *lexer, void* token);

#endif //LEXER_ANALIZADOR_LEXICO_H
