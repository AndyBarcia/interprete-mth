#ifndef LEXER_ANALIZADOR_LEXICO_H
#define LEXER_ANALIZADOR_LEXICO_H

#include <stdio.h>

typedef void* ComponenteLexico;

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
    // Nombre del fichero que se
    // está analizando, o NULL en
    // caso de estar procesando
    // un archivo o un string.
    char* nombre_fichero;
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

char* obtener_nombre_fichero(Lexer lexer);

/**
 * Devuelve el siguiente componente léxico de un analizador
 * léxico dado.
 * @param lexer analizador léxico a utilizar.
 * @param yylval_param variable en la que se guardará el token
 * @return tipo de token devuelto.
 */
int siguiente_componente_lexico(Lexer lexer, ComponenteLexico token, Localizacion *loc);

#endif //LEXER_ANALIZADOR_LEXICO_H
