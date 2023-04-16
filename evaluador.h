#ifndef PRACTICA3_EVALUADOR_H
#define PRACTICA3_EVALUADOR_H

#include "ast/ast.h"
#include "tabla_simbolos.h"

/// El contexto en el que se va a evaluar una función.
/// Dependiendo del contexto algunos tipos de expresiones
/// se permiten o no. Por ejemplo, expresiones sin efectos
/// como `5` a nivel de módulo no se permiten, pero sí en
/// modo interactivo o en modo evaluación.
typedef enum {
    /// Este es el contexto del código que se está
    /// ejecutando directamente en la terminal.
    CONTEXTO_INTERACTIVO,
    /// Este es el contexto de un módulo.
    /// Se permiten las expresiones del tipo `export`,
    /// pero fuera de las funciones y bloques sólo se
    /// permiten asignaciones.
    CONTEXTO_MODULO,
    /// Este es el contexto de una expresión evaluada
    /// dentro de una llamada a `eval`.
    CONTEXTO_EVAL
} ContextoEvaluacion;

/// Un evaluador del lenguaje matemático, con un
/// lexer y localización actual.
typedef struct {
    /// Estado del analizador léxico.
    Lexer lexer;
    /// Estado del analizador sintáctico.
    void *ps;
    /// El contexto actual.
    ContextoEvaluacion contexto;
    /// El working directory en el que se encuentra
    /// el evaluador a efectos de cargar archivos.
    /// Por defecto se puede establecer a ".".
    String wd;
} Evaluador;

/**
 * Crea un nuevo evaluador de expresiones con un
 * analizador léxico dado, que puede provenir
 * de un archivo, cadena de caracteres, o entrada
 * estándar.
 * @param lexer el analizador léxico a utilizar.
 * @param contexto el contexto de evaluación.
 * @param wd el working directory.
 * @return
 */
Evaluador crear_evaluador(Lexer lexer, ContextoEvaluacion contexto, String wd);

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

#endif //PRACTICA3_EVALUADOR_H
