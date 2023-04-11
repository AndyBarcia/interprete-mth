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

BibilotecaDinamica cargar_biblioteca_dinamica(char* fichero);
void cerrar_biblioteca_dinamica(BibilotecaDinamica *bib);

FuncionForanea cargar_funcion_biblioteca(BibilotecaDinamica bib, char* funcion);

void llamar_funcion_foranea(FuncionForanea f, ffi_type *rtype, ffi_type **arg_types, int nargs, void **args, void *rvalue);

#endif //PRACTICA3_BIBLIOTECA_DINAMICA_H
