#ifndef LEXER_ANALIZADOR_LEXICO_H
#define LEXER_ANALIZADOR_LEXICO_H

#include "tabla_simbolos.h"
#include "analizador_sintactico.h"

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
} Lexer;

/**
 * Crea un analizador léxico sobre un determinado fichero
 * ya abierto, que puede ser stdin.
 * @param fichero fichero a procesar.
 * @return el analizador léxico
 */
Lexer crear_lexer_fichero(FILE *fichero);

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
 * Devuelve el siguiente componente léxico de un analizador
 * léxico dado.
 * @param lexer analizador léxico a utilizar.
 * @param yylval_param variable en la que se guardará el token
 * @return tipo de token devuelto.
 */
int siguiente_componente_lexico(Lexer lexer, YYSTYPE *yylval_param);

#endif //LEXER_ANALIZADOR_LEXICO_H
