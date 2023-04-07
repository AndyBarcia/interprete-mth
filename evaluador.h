#ifndef PRACTICA3_EVALUADOR_H
#define PRACTICA3_EVALUADOR_H

#include "ast.h"
#include "tabla_simbolos.h"

/// Un evaluador del lenguaje matemático, con un
/// lexer y localización actual.
typedef struct {
    /// Estado del analizador léxico.
    Lexer lexer;
    /// Estado del analizador sintáctico.
    void *ps;
} Evaluador;

/**
 * Crea un nuevo evaluador de expresiones con un
 * analizador léxico dado, que puede provenir
 * de un archivo, cadena de caracteres, o entrada
 * estándar.
 * @param lexer
 * @return
 */
Evaluador crear_evaluador(Lexer lexer);

/**
 * Libera la memoria de un evaluador.
 * @param evaluador
 */
void borrar_evaluador(Evaluador *evaluador);

/**
 * Obtiene el valor resultante de evaluar la siguiente
 * expresión que hay en un evaluador.
 * @param evaluador el evaluador a usar.
 * @param tabla_simbolos la tabla de variables.
 * @param valor donde se guardará el valor.
 * @return 1 si se evaluó un valor, o 0 en caso de que
 * ya no quedasen expresiones por evaluar.
 */
int evaluar_siguiente(Evaluador *evaluador, TablaSimbolos *tabla_simbolos, Valor *valor);

/**
 * Evalúa el valor de una expresión, liberando la memoria de la expresión
 * y devolviendo su resultado en un valor.
 * @param tabla tabla de símbolos a utilizar para la resolución de variables.
 * @param exp expresión a evaluar y que será liberada.
 * @return valor de la expresión.
 */
Valor evaluar_expresion(TablaSimbolos *tabla, Expresion *exp);

/**
 * Evalúa una lista de expresiones, liberando su memoria, y devolviendo
 * sus resultados en una lista de valores.
 * @param tabla tabla de símbolos a utilizar para la resolución de variables.
 * @param listaExpresiones expresiones a evaluar y que será liberada.
 * @return lista de valores de las expresiones.
 */
ListaValores evaluar_expresiones(TablaSimbolos *tabla, ListaExpresiones *listaExpresiones);

#endif //PRACTICA3_EVALUADOR_H
