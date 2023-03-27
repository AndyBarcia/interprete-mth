#ifndef LEXER_DEFINICIONES_H
#define LEXER_DEFINICIONES_H

#include "string.h"

/// El tipo de lexema descrito en la estructura
/// ComponentLexico. Tiene que ser un carácter
/// ASCII o uno de las definiciones descritas
/// abajo (IMPORT, STRING, DECIMAL, etc).
typedef int TipoLexema;

/// Un lexema del código fuente.
typedef struct {
    /// El tipo del lexema.
    TipoLexema tipo;
    ///El lexema guardado en un string.
    String lexema;
} ComponenteLexico;

/**
 * Función que facilita la creación de un componente léxico.
 * @param tipo el tipo del componente léxico (INT, WHILE, etc).
 * @param lexema el string creado con malloc.
 * @return el componente léxico
 */
ComponenteLexico crear_componente_lexico(TipoLexema tipo, String lexema);

/**
 * Crea un error léxico utilizando el mismo formato que una
 * llamada a la función "printf". Por ejemplo:
 *
 *  int c = siguiente_caracter();
 *  crear_error_lexico("Se esperaba un dígito, pero se encontró %c", c);
 *
 * @param formato el formato del error que se pasará a printf.
 * @param ... los valores que se utilizarán para imprimir.
 * @return
 */
String crear_error_lexico(const char *formato, ...);

/**
 * Libera la memoria de un componente léxico.
 * @param componente_lexico componente léxico que será liberado. Pasará
 * de ser a tipo DESCONOCIDO y su lexema pasará a ser NULL.
 */
void borrar_componente_lexico(ComponenteLexico *componente_lexico);

/**
 * Función que imprime un componente léxico en el formato "<tipo,lexema>".
 * @param componenteLexico el componente léxico a imprimir
 */
void imprimir_componente_lexico(ComponenteLexico componenteLexico);

/**
 * Devuelve el lexema de un componente léxico.
 * @param componenteLexico
 * @return puntero a un array de caracteres.
 */
char *obtener_lexema_str(ComponenteLexico *componenteLexico);

#endif //LEXER_DEFINICIONES_H
