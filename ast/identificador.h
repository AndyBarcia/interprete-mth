#ifndef PRACTICA3_IDENTIFICADOR_H
#define PRACTICA3_IDENTIFICADOR_H

#include "../string.h"
#include "../analizador_lexico.h"

/// Un identificador, con una localización en el código fuente.
typedef struct {
    String nombre;
    Localizacion loc;
} Identificador;

/// Lista de identificadores.
typedef struct {
    int longitud;
    int capacidad;
    Identificador* valores;
} ListaIdentificadores;

/*
 * Funciones básicas de creación, borrado y clonado de identificadores.
 */

Identificador crear_identificador(String nombre, Localizacion loc);
void borrar_identificador(Identificador *identificador);
Identificador clonar_identificador(Identificador id);

/*
 * Funciones básicas de creado, borrado y clonado de listas de identificadores.
 */

ListaIdentificadores crear_lista_identificadores();
ListaIdentificadores crear_lista_identificadores1(Identificador identificador);
void push_lista_identificadores(ListaIdentificadores *lista, Identificador identificador);
ListaIdentificadores clonar_lista_identificadores(ListaIdentificadores lista);
void borrar_lista_identificadores(ListaIdentificadores *lista);

/*
 * Funciones de ayuda e impresión.
 */

char* identificador_a_str(Identificador *id);

void _imprimir_lista_identificadores(ListaIdentificadores listaIdentificadores);
void imprimir_lista_identificadores(ListaIdentificadores listaIdentificadores);

#endif //PRACTICA3_IDENTIFICADOR_H
