#ifndef LEXER_TABLA_SIMBOLOS_H
#define LEXER_TABLA_SIMBOLOS_H

#include "definiciones.h"
#include "tabla_hash.h"

typedef struct {
    TablaHash tablaHash;
} TablaSimbolos;

/**
 * Crea una nueva tabla de símbolos con las palabras claves insertadas.
 * @return nueva tabla de símbolos.
 */
TablaSimbolos crear_tabla_simbolos();

/**
 * Borra la memoria de la tabla de símbolos.
 * @param t tabla de símbolos a eliminar.
 */
void borrar_tabla_simbolos(TablaSimbolos *t);

/**
 * Busca el tipo de componente léxico correspondiente a un símbolo
 * (ya sea una palabra clave o un identificador), o inserta el
 * símbolo en la tabla de símbolos si es la primera vez que se ve.
 *
 * El símbolo se asume que es una cadena reservada con malloc, y
 * que se liberará en el caso de que ya estuviese insertada en la
 * tabla de símbolos.
 *
 * @tparam t la tabla de símbolos
 * @param simbolo la cadena que se buscará
 * @return devuelve el símbolo insertado en la tabla de símbolos.
 */
ComponenteLexico buscar_o_insertar_simbolo(TablaSimbolos *t, String simbolo);

/**
 * Imprime los símbolos de la tabla de símbolos
 * @param t la tabla de símbolos.
 */
void imprimir_simbolos(TablaSimbolos t);

#endif //LEXER_TABLA_SIMBOLOS_H
