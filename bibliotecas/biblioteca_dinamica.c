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

void llamar_funcion_foranea(FuncionForanea f, ffi_type *rtype, ffi_type **arg_types, int nargs, void **args, void *rvalue) {
    ffi_cif cif;
    ffi_status status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, nargs, rtype, arg_types);
    if (status != FFI_OK) {
        exit(1);
    }
    ffi_call(&cif, FFI_FN(f), rvalue, args);
}