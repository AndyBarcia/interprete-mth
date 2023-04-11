#ifndef PRACTICA3_STD_H
#define PRACTICA3_STD_H

#include "tabla_simbolos.h"

/**
 * Introduce en una tabla de símbolos los valores y funciones
 * inmutables de la librería estándar del lenguaje de programación.
 * @param t tabla de símbolos en la que se insertarán.
 */
void inicializar_libreria_estandar(TablaSimbolos *t);

Valor ejecutar_funcion_intrinseca(FuncionIntrinseca f, ListaValores args, TablaSimbolos *t, String wd);

#endif //PRACTICA3_STD_H
