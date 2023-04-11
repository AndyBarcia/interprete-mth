#ifndef PRACTICA3_BIBLIOTECA_DINAMICA_H
#define PRACTICA3_BIBLIOTECA_DINAMICA_H

#include "ffi.h"

/// Una biblioteca dinámica de C
typedef void* BibilotecaDinamica;

/// Una función foránea de C.
/// Por ahora tenemos la limitación de que tienen
/// que tener un prototipo fijo de 2 int de entrada
/// y 1 de salida.
typedef void* FuncionForanea;

/**
 * Trata de cargar una biblioteca dinámica de un determinado fichero.
 * @param fichero la dirección de la biblioteca.
 * @return la biblioteca, o NULL en caso de que no se haya podido cargar.
 */
BibilotecaDinamica cargar_biblioteca_dinamica(char* fichero);
/**
 * Cierra una biblioteca previamente abierta.
 * @param bib la biblioteca a cerrar.
 */
void cerrar_biblioteca_dinamica(BibilotecaDinamica *bib);

/**
 * Trata de cargar una función de un nombre dado de una biblioteca.
 * @param bib la biblioteca
 * @param funcion el nombre de la función
 * @return la función de la biblioteca, o NULL en caso de que no se
 * haya podido cargar la función.
 */
FuncionForanea cargar_funcion_biblioteca(BibilotecaDinamica bib, char* funcion);

/**
 * Llama una determinada función foránea de una biblioteca dinámica
 * con unos determinados tipos de argumentos y tipo de retorno.
 * @param f la función a ejecutar.
 * @param rtype el tipo de retorno.
 * @param arg_types array de tipos de argumentos.
 * @param nargs el número de argumentos.
 * @param args los argumentos pasados.
 * @param rvalue el lugar donde se guardará el valor de retorno.
 */
void llamar_funcion_foranea(FuncionForanea f, ffi_type *rtype, ffi_type **arg_types, int nargs, void **args, void *rvalue);

#endif //PRACTICA3_BIBLIOTECA_DINAMICA_H
