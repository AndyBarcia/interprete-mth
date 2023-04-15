#ifndef LEXER_TABLA_SIMBOLOS_H
#define LEXER_TABLA_SIMBOLOS_H

#include "tabla_hash.h"

/// Una tabla de símbolos, que es capaz de
/// almacenar valores asociados a determinadas
/// cadenas a lo largo de varios niveles o
/// `scopes`.
typedef struct {
    /// Array de tablas; una por cada nivel
    TablaHash* tablas;
    /// La capacidad actual del array de tablas.
    int capacidad;
    /// El nivel o `scope` actual.
    int nivel;
} TablaSimbolos;

/// Los distintos tipos de asignaciones que se
/// pueden hacer en la tabla de símbolos.
typedef enum {
    /// Una variable normal que puede cambiarse después.
    ASIGNACION_NORMAL,
    /// Una constante que no puede cambiarse.
    ASIGNACION_INMUTABLE,
    /// Una variable que se recibió por un import.
    /// También implica inmutabilidad.
    ASIGNACION_EXPORT
} TipoAsignacion;

/**
 * Crea una nueva tabla de símbolos con las palabras claves insertadas.
 * @return nueva tabla de símbolos.
 */
TablaSimbolos crear_tabla_simbolos();

/**
 * Aumenta el nivel o `scope` de la tabla de símbolos. Las variables
 * que se inserten en este nuevo nivel se liberarán automáticamente
 * cuando se haga la llamada correspondiente a `reducir_nivel_tabla_simbolos`.
 * @param t la tabla de símbolos
 */
void aumentar_nivel_tabla_simbolos(TablaSimbolos *t);

/**
 * Libera la memoria asociada al último nivel de la tabla de símbolos-
 * @param t
 */
void reducir_nivel_tabla_simbolos(TablaSimbolos *t);

/**
 * Borra la memoria de la tabla de símbolos.
 * @param t tabla de símbolos a eliminar.
 */
void borrar_tabla_simbolos(TablaSimbolos *t);

/**
 * Recupera un valor de la tabla de símbolos con un nombre dado.
 * Los valores se recuperan mirando primero en los últimos niveles creados,
 * dando preferencia así a las variables locales sobre a las globales.
 * @param t la tabla de símbolos en la que se buscará.
 * @param nombre el nombre de la variable
 * @param valor el lugar donde se guardará el clon del valor que hay en la
 * tabla. Puede ser NULL, en cuyo caso no se creará el clon.
 * @param tipo el lugar donde se guardará el tipo de asignación que tenía
 * el valor. Puede ser NULL, en cuyo caso no se guardará el tipo.
 * @return devuelve 1 si existía el valor, y 0 en caso contrario.
 */
int recuperar_clon_valor_tabla(TablaSimbolos t, String nombre, Valor *valor, TipoAsignacion *tipo);

/**
 * Asigna un determinado valor a un nombre dado en el último nivel de la tabla
 * de símbolos, utilizando un tipo de asignación dado.
 * @param t la tabla de símbolos.
 * @param nombre el nombre al que se asignará la variable.
 * @param valor el valor a asignar.
 * @param tipo el tipo de asignación
 * @return 1 en caso de que se haya asignado correctamente, o 0 en caso
 * contrario (por ejemplo, en caso de tratando de asignar una variable
 * inmutable definida previamente).
 */
int asignar_valor_tabla(TablaSimbolos *t, String nombre, Valor valor, TipoAsignacion tipo);

/**
 * Clona todos los valores de una tabla hash dada al últivo nivel de
 * esta tabla de símbolos.
 * @param t la tabla de símbolos en la que se asignarán los valores.
 * @param otro la tabla con el conjunto de valores.
 * @return devuelve el número de elementos de la tabla hash que se
 * pudieron insertar correctamente.
 */
int asignar_clones_valores_tabla(TablaSimbolos *t, TablaHash otro);

/**
 * Imprime los símbolos de la tabla de símbolos
 * @param t la tabla de símbolos.
 */
void imprimir_simbolos(TablaSimbolos t);

#endif //LEXER_TABLA_SIMBOLOS_H
