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

Identificador crear_identificador(String nombre, Localizacion loc);
void borrar_identificador(Identificador *identificador);
Identificador clonar_identificador(Identificador id);
char* identificador_a_str(Identificador *id);

ListaIdentificadores crear_lista_identificadores();
void push_lista_identificadores(ListaIdentificadores *lista, Identificador identificador);
ListaIdentificadores clonar_lista_identificadores(ListaIdentificadores lista);
void borrar_lista_identificadores(ListaIdentificadores *lista);

void _imprimir_lista_identificadores(ListaIdentificadores listaIdentificadores);
void imprimir_lista_identificadores(ListaIdentificadores listaIdentificadores);

#endif //PRACTICA3_IDENTIFICADOR_H
