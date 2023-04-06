#ifndef PRACTICA3_ERROR_H
#define PRACTICA3_ERROR_H

#include "string.h"
#include "analizador_lexico.h"

/// Tipo interno de un error.
typedef struct {
    String mensaje;
} Error;

Error crear_error(const char *formato, ...);
void borrar_error(Error *error);

void imprimir_error(Error error, char* nombre, char* linea, Localizacion *loc);

#endif //PRACTICA3_ERROR_H
