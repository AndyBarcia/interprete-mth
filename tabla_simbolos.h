#ifndef LEXER_TABLA_SIMBOLOS_H
#define LEXER_TABLA_SIMBOLOS_H

#include "tabla_hash.h"
#include "ast.h"

typedef struct {
    // Array de tablas; una por cada nivel
    TablaHash* tablas;
    int capacidad;
    int nivel;
} TablaSimbolos;

/**
 * Crea una nueva tabla de símbolos con las palabras claves insertadas.
 * @return nueva tabla de símbolos.
 */
TablaSimbolos crear_tabla_simbolos();

void aumentar_nivel_tabla_simbolos(TablaSimbolos *t);

void reducir_nivel_tabla_simbolos(TablaSimbolos *t);

/**
 * Borra la memoria de la tabla de símbolos.
 * @param t tabla de símbolos a eliminar.
 */
void borrar_tabla_simbolos(TablaSimbolos *t);

Valor recuperar_valor_tabla(TablaSimbolos t, String identificador);

int asignar_valor_tabla(TablaSimbolos *t, String identificador, Valor valor, int inmutable);

int asignar_clones_valores_tabla(TablaSimbolos *t, TablaHash otro);

/**
 * Imprime los símbolos de la tabla de símbolos
 * @param t la tabla de símbolos.
 */
void imprimir_simbolos(TablaSimbolos t);

#endif //LEXER_TABLA_SIMBOLOS_H
