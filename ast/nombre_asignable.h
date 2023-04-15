#ifndef PRACTICA3_NOMBRE_ASIGNABLE_H
#define PRACTICA3_NOMBRE_ASIGNABLE_H

#include "identificador.h"

/// Tipos de acceso
typedef enum {
    /// Accesos del tipo `x.y`
    ACCESO_MIEMBRO,
    /// Accesos del tipo `x[y]`
    ACCESO_INDEXADO
} TipoAcceso;

typedef struct {
    TipoAcceso tipo;
    union {
        Identificador miembro;
        struct Expresion* indice;
    };
} Acceso;

/// El nombre de un lugar que puede ser asignado, como es el
/// caso de `x`, `x.y`, `x[y]`, o cualquier combinación de ellos.
typedef struct {
    Identificador nombre_base;
    int longitud;
    int capacidad;
    Acceso* accesos;
} NombreAsignable;

NombreAsignable crear_nombre_asignable(Identificador nombre);
NombreAsignable clonar_nombre_asignable(NombreAsignable nombre);
void borrar_nombre_asignable(NombreAsignable *nombre);

Acceso crear_acceso_miembro(Identificador miembro);
Acceso crear_acceso_indexado(struct Expresion* indice);
Acceso clonar_acceso(Acceso acceso);
void borrar_acceso(Acceso *acceso);

void push_acceso_nombre_asignable(NombreAsignable *nombre, Acceso acceso);

void imprimir_acceso(Acceso acceso);
void imprimir_nombre_asignable(NombreAsignable nombre);

#endif //PRACTICA3_NOMBRE_ASIGNABLE_H
