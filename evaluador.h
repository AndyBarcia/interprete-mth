#ifndef PRACTICA3_EVALUADOR_H
#define PRACTICA3_EVALUADOR_H

#include "ast/ast.h"
#include "tabla_simbolos.h"

/// El contexto en el que se va a ejecutar una función.
/// Dependiendo del contexto algunos tipos de expresiones
/// se permiten o no.
typedef enum {
    /// Este es el contexto del código que se está
    /// ejecutando directamente en la terminal.
    CNTXT_INTERACTIVO,
    /// Este es el contexto de una expresión que
    /// se va a asignar en una asignación.
    CNTXT_ASIGNACION,
    /// Este es el contexto de las expresiones
    /// que se van a pasar como argumentos a una función.
    CNTXT_ARGS,
    /// Este es el contexto de un módulo.
    /// Se permiten las expresiones del tipo `export`.
    CNTXT_MODULO,
    /// Este es el contexto de una función.
    /// Se permiten expresiones del tipo `return`
    CNTXT_FUNCION,
    /// Este es el contexto de un bucle.
    /// Se permiten expresiones del tipo `break`.
    CNTXT_BUCLE,
} Contexto;

/// Un evaluador del lenguaje matemático, con un
/// lexer y localización actual.
typedef struct {
    /// Estado del analizador léxico.
    Lexer lexer;
    /// Estado del analizador sintáctico.
    void *ps;
    /// El contexto actual.
    Contexto contexto;
} Evaluador;

/**
 * Crea un nuevo evaluador de expresiones con un
 * analizador léxico dado, que puede provenir
 * de un archivo, cadena de caracteres, o entrada
 * estándar.
 * @param lexer
 * @return
 */
Evaluador crear_evaluador(Lexer lexer, Contexto contexto);

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
Valor evaluar_expresion(TablaSimbolos *tabla, Expresion *exp, Contexto contexto);

/**
 * Evalúa una lista de expresiones, liberando su memoria, y devolviendo
 * sus resultados en una lista de valores.
 * @param tabla tabla de símbolos a utilizar para la resolución de variables.
 * @param listaExpresiones expresiones a evaluar y que será liberada.
 * @return lista de valores de las expresiones.
 */
ListaValores evaluar_expresiones(TablaSimbolos *tabla, ListaExpresiones *listaExpresiones, Contexto contexto);

#endif //PRACTICA3_EVALUADOR_H
