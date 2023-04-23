#ifndef LEXER_ANALIZADOR_LEXICO_H
#define LEXER_ANALIZADOR_LEXICO_H

#include <stdio.h>
#include "codigo_fuente.h"

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
    // Posición actual del analizador.
    Localizacion loc;
} Lexer;

/**
 * Crea un analizador léxico sobre un determinado
 * código fuente.
 * @param codigo_fuente datos a procesar.
 * @return el analizador léxico creado.
 */
Lexer crear_lexer(CodigoFuente codigo_fuente);

/**
 * Libera la memoria de un analizador léxico dado.
 * @param lexer
 */
void borrar_analizador_lexico(Lexer *lexer);

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
