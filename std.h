#ifndef PRACTICA3_STD_H
#define PRACTICA3_STD_H

#include "tabla_simbolos.h"

/**
 * Introduce en una tabla de símbolos los valores y funciones
 * inmutables de la librería estándar del lenguaje de programación.
 * @param t tabla de símbolos en la que se insertarán.
 */
void inicializar_libreria_estandar(TablaSimbolos *t);

/**
 * Ejecuta una determina función intrínseca con una lista de
 * argumentos dados, con una determinada tabla de símbolos,
 * y en un directorio de trabajo dado.
 * @param f la función intrínseca a ejecutar.
 * @param args la lista de argumentos.
 * @param t la tabla de símbolos.
 * @param wd el working directory o directorio de trabajo.
 * @return el valor devuelto por la función.
 */
Valor ejecutar_funcion_intrinseca(FuncionIntrinseca f, ListaValores args, TablaSimbolos *t, String wd);

#endif //PRACTICA3_STD_H
