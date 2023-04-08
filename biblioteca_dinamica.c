#include "biblioteca_dinamica.h"
#include <stdlib.h>
#include <dlfcn.h>

BibilotecaDinamica cargar_biblioteca_dinamica(char* fichero) {
    return dlopen(fichero, RTLD_LAZY);
}

void cerrar_biblioteca_dinamica(BibilotecaDinamica *bib) {
    dlclose(*bib);
    *bib = NULL;
}

FuncionForanea cargar_funcion_biblioteca(BibilotecaDinamica bib, char* funcion) {
    return dlsym(bib, funcion);
}