#ifndef PRACTICA3_IDENTIFICADOR_H
#define PRACTICA3_IDENTIFICADOR_H

#include "../string.h"
#include "../analizador_lexico.h"

/// Un identificador, con una localización en el código fuente.
typedef struct {
    /// El nombre del identificador.
    String nombre;
    /// La localización del identificador
    /// en el código fuente
    Localizacion loc;
} Identificador;

/// Una lista de identificadores.
typedef struct {
    int longitud;
    int capacidad;
    Identificador* valores;
} ListaIdentificadores;

/**
 * Crea un nuevo identificador con un nombre y ubicación dados.
 * @param nombre El nombre del identificador.
 * @param loc La ubicación del identificador en el código fuente.
 * @return El nuevo identificador.
 */
Identificador crear_identificador(String nombre, Localizacion loc);

/**
 * Borra un identificador y libera su memoria asociada.
 * @param identificador El identificador a borrar.
 */
void borrar_identificador(Identificador *identificador);

/**
 * Clona profundo de un identificador.
 * @param id El identificador a clonar.
 * @return El nuevo identificador clonado.
 */
Identificador clonar_identificador(Identificador id);

/**
 * Crea una nueva lista vacía de identificadores.
 * @return La nueva lista de identificadores.
 */
ListaIdentificadores crear_lista_identificadores();

/**
 * Crea una nueva lista de identificadores con un único identificador.
 * @param identificador El identificador a agregar a la lista.
 * @return La nueva lista de identificadores.
 */
ListaIdentificadores crear_lista_identificadores1(Identificador identificador);

/**
 * Agrega un identificador a una lista de identificadores.
 * @param lista La lista de identificadores a la que se agregará el identificador.
 * @param identificador El identificador a agregar a la lista.
 */
void push_lista_identificadores(ListaIdentificadores *lista, Identificador identificador);

/**
 * Clonado profundo de una lista de identificadores.
 * @param lista La lista de identificadores a clonar.
 * @return La nueva lista de identificadores clonada.
 */
ListaIdentificadores clonar_lista_identificadores(ListaIdentificadores lista);

/**
 * Borra una lista de identificadores y libera su memoria asociada.
 * @param lista La lista de identificadores a borrar.
 */
void borrar_lista_identificadores(ListaIdentificadores *lista);

/**
 * Convierte un identificador en una cadena de caracteres.
 * @param id El identificador a convertir.
 * @return La cadena de caracteres correspondiente al identificador.
 */
char* identificador_a_str(Identificador *id);

/**
 * Imprime una lista de identificadores en la salida estándar,
 * separados por comas.
 * Es utilizado para debugging.
 * @param listaIdentificadores La lista de identificadores a imprimir.
 */
void imprimir_lista_identificadores(ListaIdentificadores listaIdentificadores);

#endif //PRACTICA3_IDENTIFICADOR_H
