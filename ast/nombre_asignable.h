#ifndef PRACTICA3_NOMBRE_ASIGNABLE_H
#define PRACTICA3_NOMBRE_ASIGNABLE_H

#include "identificador.h"

/// Tipos de acceso a una expresión
typedef enum {
    /// Acceso del tipo `x.y`
    ACCESO_MIEMBRO,
    /// Acceso del tipo `x[y]`
    ACCESO_INDEXADO
} TipoAcceso;

/// Un acceso de un tipo dado.
typedef struct {
    TipoAcceso tipo;
    union {
        /// El nombre del miembro
        /// accedido con `x.y`
        Identificador miembro;
        /// La expresión utilizada
        /// como índice en un acceso
        /// del tipo `x[y]`.
        struct Expresion* indice;
    };
} Acceso;

/// El nombre de un lugar que puede ser asignado o accedido.
/// Está formado por un identificador base y una seria de
/// accesos de tipo `x.y` o `x[y]`.
/// Por ejemplo: `x.y.z` o `x[y][z]` o cualquier combinación
/// de accesos.
typedef struct {
    Identificador nombre_base;
    int longitud;
    int capacidad;
    Acceso* accesos;
} NombreAsignable;

/**
 * Crea un nuevo nombre asignable con un identificador
 * como nombre base.
 * @param nombre
 * @return
 */
NombreAsignable crear_nombre_asignable(Identificador nombre);

/**
 * Clonado profundo de un nombre asignable.
 * @param nombre
 * @return
 */
NombreAsignable clonar_nombre_asignable(NombreAsignable nombre);

/**
 * Borra un nombre asignable y memoria su memoria
 * asignada.
 * @param nombre
 */
void borrar_nombre_asignable(NombreAsignable *nombre);

/**
 * Crea un nuevo acceso de tipo ACCESO_MIEMBRO a un miembro
 * de un nombre asignable.
 * @param miembro El identificador del miembro al que se
 * accederá.
 * @return El acceso creado.
 */
Acceso crear_acceso_miembro(Identificador miembro);

/**
 * Crea un nuevo acceso de tipo ACCESO_INDEXADO a un índice
 * de un nombre asignable.
 * @param indice La expresión que se utilizará como índice.
 * @return El acceso creado.
 */
Acceso crear_acceso_indexado(struct Expresion* indice);

/**
 * Clona profundo un acceso.
 * @param acceso El acceso a clonar.
 * @return El acceso clonado.
 */
Acceso clonar_acceso(Acceso acceso);

/**
 * Borra un acceso y libera su memoria asignada.
 * @param acceso El acceso a borrar.
 */
void borrar_acceso(Acceso *acceso);

/**
 * Añade un nuevo acceso a un nombre asignable.
 * @param nombre el nombre asignable al que se
 * le añadirá el acceso.
 * @param acceso el nuevo acceso.
 */
void push_acceso_nombre_asignable(NombreAsignable *nombre, Acceso acceso);

/**
 * Imprime un acceso en la consola.
 * Es utilizado para debugging.
 * @param acceso El acceso a imprimir.
 */
void imprimir_acceso(Acceso acceso);

/**
 * Imprime un nombre asignable en la consola.
 * Es utilizado para debugging.
 * @param nombre El nombre asignable a imprimir.
 */
void imprimir_nombre_asignable(NombreAsignable nombre);

#endif //PRACTICA3_NOMBRE_ASIGNABLE_H
