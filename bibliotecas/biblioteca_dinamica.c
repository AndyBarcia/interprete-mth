#include "biblioteca_dinamica.h"
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

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

void _llamar_funcion_foranea(FuncionForanea f, ffi_type *rtype, ffi_type **arg_types, int nargs, void **args, void *rvalue) {
    ffi_cif cif;
    ffi_status status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, nargs, rtype, arg_types);
    if (status != FFI_OK) {
        exit(1);
    }
    ffi_call(&cif, FFI_FN(f), rvalue, args);
}

Valor llamar_funcion_foranea(FuncionForanea f, TipoForaneo tipo_retorno, Valor* vargs, int nargs) {
    ffi_type **arg_types = malloc(sizeof (ffi_type) * nargs);
    void **args = malloc(sizeof(void*) * nargs);
    for (int i = 0; i < nargs; ++i) {
        switch (vargs[i].tipo_valor) {
            case TIPO_ENTERO:
                arg_types[i] = &ffi_type_sint;
                args[i] = &vargs[i].entero;
                break;
            case TIPO_DECIMAL:
                arg_types[i] = &ffi_type_double;
                args[i] = &vargs[i].decimal;
                break;
            case TIPO_BOOL:
                arg_types[i] = &ffi_type_ushort;
                args[i] = &vargs[i].bool;
                break;
            case TIPO_STRING:
                arg_types[i] = &ffi_type_pointer;
                char* ptr = string_a_str(&vargs[i].string);
                args[i] = &ptr;
                break;
            case TIPO_FUNCION_FORANEA:
                arg_types[i] = &ffi_type_pointer;
                args[i] = &vargs[i].funcion_foranea;
                break;
            default: {
                free(arg_types);
                free(args);
                Error error = crear_error("No se puede pasar un valor de tipo %s como argumento a una función foránea.",
                                          tipo_valor_a_str(vargs[i].tipo_valor));
                return crear_valor_error(error, vargs[i].loc);
            }
        }
    }

    Valor resultado;
    switch (tipo_retorno) {
        case TIPO_C_INT: {
            int rvalue;
            _llamar_funcion_foranea(f, &ffi_type_sint, arg_types, nargs, args, &rvalue);
            resultado = crear_entero(rvalue, NULL);
            break;
        }
        case TIPO_C_FLOAT: {
            float rvalue;
            _llamar_funcion_foranea(f, &ffi_type_float, arg_types, nargs, args, &rvalue);
            resultado = crear_decimal((Decimal) rvalue, NULL);
            break;
        }
        case TIPO_C_DOUBLE: {
            double rvalue;
            _llamar_funcion_foranea(f, &ffi_type_double, arg_types, nargs, args, &rvalue);
            resultado = crear_decimal((Decimal) rvalue, NULL);
            break;
        }
        case TIPO_C_VOID: {
            _llamar_funcion_foranea(f, &ffi_type_void, arg_types, nargs, args, NULL);
            resultado = crear_valor_unidad(NULL);
            break;
        }
    }

    free(arg_types);
    free(args);
    return resultado;

}