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

/// Un tipo de C.
typedef enum {
    TIPO_C_INT,
    TIPO_C_FLOAT,
    TIPO_C_DOUBLE,
    TIPO_C_VOID
} TipoForaneo;

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

#include "../ast/valor.h"

/**
 * Llama una determinada función foránea con unos argumentos y tipo de retorno
 * dados. El prototipo de la función foránea se determina automáticamente en
 * base a los tipos de los argumentos recibidos y el tipo de retorno.
 * @param f la función foránea a ejecutar.
 * @param tipo_retorno el tipo de retorno de la función.
 * @param vargs los valores que se pasarán como argumentos.
 * @param nargs el número de argumentos.
 * @return el resultado de ejecutar la función, o un error en caso de
 * que se haya producido algún problema al llamar a la función.
 */
Valor llamar_funcion_foranea(FuncionForanea f, TipoForaneo tipo_retorno, Valor* vargs, int nargs);

#endif //PRACTICA3_BIBLIOTECA_DINAMICA_H
