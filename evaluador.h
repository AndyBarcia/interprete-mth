#ifndef PRACTICA3_EVALUADOR_H
#define PRACTICA3_EVALUADOR_H

#include "ast/ast.h"
#include "tabla_simbolos.h"

/// El contexto en el que se va a evaluar una expresión.
/// Dependiendo del contexto algunos tipos de expresiones
/// se permiten o no. Por ejemplo, expresiones sin efectos
/// como `5` a nivel de módulo no se permiten, pero sí en
/// modo interactivo o en modo evaluación. O expresiones como
/// `export` sólo se permiten a nivel de módulo.
typedef enum {
    /// Este es el contexto del código que se está
    /// ejecutando directamente en la terminal.
    CONTEXTO_INTERACTIVO,
    /// Este es el contexto de un módulo.
    /// A este nivel sólo se permiten 2 tipos de expresiones:
    ///   * Imports, para importar otros módulos.
    ///   * Asignaciones, para establecer variables locales
    ///     o exportar valores fuera del módulo.
    CONTEXTO_MODULO,
    /// Este es el contexto de una expresión evaluada
    /// dentro de una llamada a `eval`.
    /// Por ahora funciona como el contexto interactivo, pero
    /// se podrían limitar las funciones que interactúan con
    /// el sistema operativo, por ejemplo, para aumentar su
    /// seguridad.
    CONTEXTO_EVAL
} ContextoParsing;

/// Un analizador sintáctico del lenguaje
/// matemático, que permite extraer
/// expresiones.
typedef struct {
    /// Estado actual del analizador léxico.
    Lexer lexer;
    /// Estado actual del analizador sintáctico.
    void *ps;
    /// El contexto en el que se están analizando
    /// expresiones. Dependiendo de este valor se
    /// permiten o no ciertas expresiones.
    ContextoParsing contexto;
} Parser;

/// Un evaluador del lenguaje matemático.
typedef struct {
    /// La tabla de símbolos con los valores actuales
    /// de las variables.
    TablaSimbolos *tabla_simbolos;
    /// El working directory en el que se encuentra
    /// el evaluador a efectos de cargar archivos.
    /// Por defecto se puede establecer a ".".
    String wd;
} Evaluador;

/**
 * Crea un nuevo parser dado un analizador léxico dado,
 * en un contexto dado.
 * @param lexer el analizador léxico.
 * @param contexto el contexto de parsing.
 * @return
 */
Parser crear_parser(Lexer lexer, ContextoParsing contexto);

/**
 * Libera la memoria asociada a un parser.
 * @param parser
 */
void borrar_parser(Parser *parser);

/**
 * Extrae la siguiente expresión de un parser dado.
 * @param parser el analizador sintáctico
 * @param expresion donde se guardará la expresión
 * @return 1 si se consiguió una nueva expresión, o
 * 0 si ya no quedan más expresiones.
 */
int siguiente_expresion(Parser *parser, Expresion *expresion);

/**
 * Crea un nuevo evaluador de expresiones con un
 * con una tabla de símbols dada en un directorio
 * de trabajo.
 * @param tabla_simbolos la tabla de símbolos.
 * @param wd el working directory.
 * @return
 */
Evaluador crear_evaluador(TablaSimbolos *tabla_simbolos, String wd);

/**
 * Libera la memoria de un evaluador.
 * @param evaluador
 */
void borrar_evaluador(Evaluador *evaluador);

/**
 * Obtiene el valor resultante de evaluar una expresión dada.
 * La memoria de la expresión queda liberada.
 * @param evaluador el evaluador a usar.
 * @param exp la expresión a evaluar.
 * @return el valor resultante.
 */
Valor evaluar_expresion(Evaluador *evaluador, Expresion *exp);

#endif //PRACTICA3_EVALUADOR_H
