#ifndef LEXER_TABLA_HASH_H
#define LEXER_TABLA_HASH_H

#include "definiciones.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    TipoLexema tipo;
    String clave;
    int distancia_posicion_ideal;
    unsigned int hash;
} EntradaTablaHash;

/**
 * Tabla hash según la implementación descrita en
 * https://probablydance.com/2017/02/26/i-wrote-the-fastest-hashtable/
 * Es una tabal hash:
 *  - Con un bloque de memoria continuo.
 *  - Búsqueda lineal con Robing Hood y límite de búsqueda
 *  - Tamaño primo
 */
typedef struct {
    EntradaTablaHash *buffer;
    int longitud;
    int capacidad;
    int limite_busqueda;
} TablaHash;

/**
 * Inicializa cada elemento de la tabla a un contenido vacío.
 * El hashmap no utilizará la capacidad exacta pasada al método
 * sino un primo cercano mayor o igual que la capacidad.
 * @param capacidad capacidad inicial del hashmap.
 * @return
 */
TablaHash crear_tabla_hash(int capacidad);

/**
 * Borra la memoria de la tabla hash.
 * @param t
 */
void borrar_tabla_hash(TablaHash *t);

/**
 * Comprobar si clavebuscar está en la tabla hash
 * @param t es la tabla hash en la que buscamos la clave
 * @param clavebuscar: es la clave que buscamos
 * @return si la clave está en la tabla
 */
int es_miembro_hash(TablaHash t, char *clavebuscar);

/**
 * Buscar una clave en la tabla hash
 * @param t es la tabla hash en la que buscamos la clave
 * @param lexema: es la clave que buscamos
 * @param entrada aquí se pondrá la entrada encontrada.
 *        No se modificará si no se encontró la clave.
 * @return si la búsqueda tuvo éxito
 */
int buscar_hash(TablaHash t, char *lexema, ComponenteLexico *entrada);

/**
 * Inserta un elemento en una tabla hash al principio de la lista.
 * Si ya había un lexema en la tabla hash con esa misma clave, se
 * libera la memoria del string que ya estaba en la tabla hash, y
 * se sustituye por el componente léxico que se pasó como argumento.
 * @param t la tabla hash
 * @param ComponenteLexico el componente léxico a insertar.
 * @return el elemento que se ha insertado.
 */
ComponenteLexico insertar_hash(TablaHash *t, ComponenteLexico lexema);

/**
 * Trata de insertar un elemento en la tabla hash si este no está
 * presente, o devuelve el elemento que ya estaba en la tabla sin
 * modificar la tabla.
 * @param t la tabal hash.
 * @param lexema el componente léxico a insertar.
 * @param resultado donde se guardará el componente léxico que
 *        ya se encontraba en la tabla. No se modificará si no
 *        estaba en la tabla.
 * @return 1 si ya estaba el elemento en la tabla, o 0 en caso
 *        contrario.
 */
int buscar_o_insertar_hash(TablaHash *t, ComponenteLexico lexema, ComponenteLexico *resultado);

/**
 * Borra un elemento en una tabla hash
 * @param t es la tabla hash
 * @param claveborrar es el elemento que queremos borrar en la tabla
 * @return 1 si se eliminó un elemento o 0 en caso contrario.
 */
int borrar_hash(TablaHash *t, char *claveborrar);

/**
 * Ejecuta una función por cada elemento de la tabla hash.
 * @param t es la tabla hash
 * @param visitador función a ejecutar
 */
void iterar_tabla_hash(TablaHash t, void (*visitador)(EntradaTablaHash entrada));

void imprimir_tabla_hash(TablaHash t);

#endif //LEXER_TABLA_HASH_H
