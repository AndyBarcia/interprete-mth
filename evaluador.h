#ifndef PRACTICA3_EVALUADOR_H
#define PRACTICA3_EVALUADOR_H

#include "ast.h"
#include "tabla_simbolos.h"

/**
 * Evalúa el código de un fichero dado, devolviendo el último valor resulto.
 * @param tabla tabla de símbolos a utilizar para la resolución de variables.
 * @param entrada fichero a evaluar.
 * @return último valor.
 */
Valor evaluar_fichero(TablaSimbolos *tabla, FILE *entrada);

/**
 * Evalúa el código de un archivo dado, devolviendo el último valor resulto
 * o un valor de error en caso de que no se haya conseguido abrir el archivo.
 * @param tabla tabla de símbolos a utilizar para la resolución de variables.
 * @param archivo fichero a evaluar.
 * @return último valor.
 */
Valor evaluar_archivo(TablaSimbolos *tabla, char* archivo);

/**
 * Evalúa el código de una cadena dada, devolviendo su valor.
 * @param tabla tabla de símbolos a utilizar para la resolución de variables.
 * @param str cadena a evaluar.
 * @return valor de la cadena.
 */
Valor evaluar_str(TablaSimbolos *tabla, char* str);

/**
 * Evalúa el valor de una expresión, liberando la memoria de la expresión
 * y devolviendo su resultado en un valor.
 * @param tabla tabla de símbolos a utilizar para la resolución de variables.
 * @param exp expresión a evaluar y que será liberada.
 * @return valor de la expresión.
 */
Valor evaluar_expresion(TablaSimbolos *tabla, Expresion *exp);

/**
 * Evalúa una lista de expresiones, liberando su memoria, y devolviendo
 * sus resultados en una lista de valores.
 * @param tabla tabla de símbolos a utilizar para la resolución de variables.
 * @param listaExpresiones expresiones a evaluar y que será liberada.
 * @return lista de valores de las expresiones.
 */
ListaValores evaluar_expresiones(TablaSimbolos *tabla, ListaExpresiones *listaExpresiones);

#endif //PRACTICA3_EVALUADOR_H
