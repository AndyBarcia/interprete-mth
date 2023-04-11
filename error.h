#ifndef PRACTICA3_ERROR_H
#define PRACTICA3_ERROR_H

#include "string.h"
#include "analizador_lexico.h"
#include "ast/control_flujo.h"

/// Tipo interno de un error.
typedef struct {
    String mensaje;
} Error;

/*
 * Funciones de ayuda de creación de errores.
 */

Error crear_error(const char *formato, ...);
Error crear_error_numero_argumentos(int esperado, int actual);
Error crear_error_tipos_incompatibles(char* op, int tipoA, int tipoB);
Error crear_error_op_incompatible(char* op, int tipo);
Error crear_error_dividir_entre_cero();
Error crear_error_archivo_no_encontrado(char* fichero);
Error crear_error_contexto_incorrecto(char* tipo, char* contexto);
Error crear_error_casting(int tipoActual, int tipoObjetivo);

void borrar_error(Error *error);

void imprimir_error(Error error, char* nombre, char* linea, Localizacion *loc);

#endif //PRACTICA3_ERROR_H
