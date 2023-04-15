#include "nombre_asignable.h"
#include "ast.h"
#include <stdlib.h>

NombreAsignable crear_nombre_asignable(Identificador nombre) {
    return (NombreAsignable) {
        .nombre_base = nombre,
        .longitud = 0,
        .capacidad = 0,
        .accesos = NULL
    };
}

NombreAsignable clonar_nombre_asignable(NombreAsignable nombre) {
    NombreAsignable copia = (NombreAsignable) {
        .nombre_base = clonar_identificador(nombre.nombre_base),
        .capacidad = nombre.longitud,
        .longitud = nombre.longitud,
        .accesos = malloc(sizeof(Acceso)*nombre.longitud)
    };
    for (int i = 0; i < copia.longitud; ++i) {
        copia.accesos[i] = clonar_acceso(nombre.accesos[i]);
    }
    return copia;
}

void borrar_nombre_asignable(NombreAsignable *nombre) {
    borrar_identificador(&nombre->nombre_base);
    if (nombre->accesos) {
        for (int i = 0; i < nombre->longitud; ++i)
            borrar_acceso(&nombre->accesos[i]);
        free(nombre->accesos);
        nombre->accesos = NULL;
        nombre->longitud = 0;
        nombre->capacidad = 0;
    }
}

Acceso crear_acceso_miembro(Identificador miembro) {
    return (Acceso) {
        .tipo = ACCESO_MIEMBRO,
        .miembro = miembro
    };
}

Acceso crear_acceso_indexado(struct Expresion* indice) {
    Expresion* e = malloc(sizeof(Expresion));
    *e = *(Expresion*) indice;

    return (Acceso) {
        .tipo = ACCESO_INDEXADO,
        .indice = (struct Expresion*) e
    };
}

Acceso clonar_acceso(Acceso acceso) {
    Acceso copia = acceso;
    switch (copia.tipo) {
        case ACCESO_MIEMBRO:
            copia.miembro = clonar_identificador(acceso.miembro);
            break;
        case ACCESO_INDEXADO:
            copia.indice = malloc(sizeof(Expresion));
            *(Expresion*) copia.indice = clonar_expresion(*(Expresion*) acceso.indice);
            break;
    }
    return copia;
}

void borrar_acceso(Acceso *acceso) {
    switch (acceso->tipo) {
        case ACCESO_MIEMBRO:
            borrar_identificador(&acceso->miembro);
            break;
        case ACCESO_INDEXADO:
            borrar_expresion((Expresion *) acceso->indice);
            free(acceso->indice);
            break;
    }
}

void push_acceso_nombre_asignable(NombreAsignable *nombre, Acceso acceso) {
    if (nombre->longitud >= nombre->capacidad) {
        nombre->accesos = realloc(nombre->accesos, (nombre->capacidad + 1) * sizeof(Acceso));
        ++nombre->capacidad;
    }
    nombre->accesos[nombre->longitud++] = acceso;
}

void _imprimir_expresion(Expresion expresion);

void _imprimir_acceso(Acceso acceso) {
    switch (acceso.tipo) {
        case ACCESO_MIEMBRO:
            printf(".%s", string_a_puntero(&acceso.miembro.nombre));
            break;
        case ACCESO_INDEXADO:
            printf("[");
            _imprimir_expresion(*(Expresion*) acceso.indice);
            printf("]");
            break;
    }
}

void imprimir_acceso(Acceso acceso) {
    _imprimir_acceso(acceso);
    printf("\n");
}

void _imprimir_nombre_asignable(NombreAsignable nombre) {
    printf("%s", string_a_puntero(&nombre.nombre_base.nombre));
    if (nombre.accesos) {
        for (int i = 0; i < nombre.longitud; ++i)
            _imprimir_acceso(nombre.accesos[i]);
    }
}

void imprimir_nombre_asignable(NombreAsignable nombre) {
    _imprimir_nombre_asignable(nombre);
    printf("\n");
}