#ifndef PRACTICA3_AST_H
#define PRACTICA3_AST_H

#include "string.h"
#include "valor.h"
#include "analizador_lexico.h"
#include "tabla_simbolos.h"

/// Una lista de expresiones, ya sea en un
/// bloque de expresiones o los argumentos
/// de una llamada a una función.
typedef struct {
    int capacidad;
    int longitud;
    struct Expresion* valores;
    Localizacion loc;
} ListaExpresiones;

/// Llamada a una expresión como si fuese
/// una función, utilizando uns determinados
/// argumentos.
typedef struct {
    struct Expresion *funcion;
    ListaExpresiones argumentos;
    Localizacion loc;
} LlamadaFuncion;

/// Asignación del valor de una expresión
/// a un identificador.
typedef struct {
    /// El nombre al que se va a asignar
    /// el valor de la expresión.
    Identificador identificador;
    /// La expresión a evaluar.
    struct Expresion *expresion;
    /// Si la asignación es normal, o
    /// si es de tipo inmutable o export.
    /// Ejemplo: `const x = 5`
    /// Ejemplo: `export pi = 3.14`
    TipoAsignacion tipo;
    /// La localización de la expresión
    /// en el código fuente.
    Localizacion loc;
} Asignacion;

/// La definición de una función creada por
/// el usuario.
typedef struct {
    ListaIdentificadores argumentos;
    struct Expresion *cuerpo;
    Localizacion loc;
} DefinicionFuncion;

/// La importación de un archivo, ya sea
/// del propio lenguaje, o una biblioteca
/// foránea con funciones de C.
typedef struct {
    String archivo;
    int foraneo;
    Localizacion loc;
} Import;

/// El tipo de una expresión.
typedef enum {
    /// Una expresión desconocida.
    /// Sólo se utiliza internamente en el código.
    EXP_NULA,
    /// Una expresión que es un simple valor.
    /// Ejemplo: `5`
    EXP_VALOR,
    /// Una expresión que es un simple identificador.
    /// Ejemplo: `x`
    EXP_IDENTIFICADOR,
    /// Una expresión que es una llamada a una función
    /// con ciertos argumentos.
    /// Ejemplo: `f(5)`, `(\x=>x+1)(2)`
    EXP_OP_LLAMADA,
    /// Una expresión que es una asignación de un valor
    /// a un identificador.
    /// Ejemplo: `x = 5`, `const pi = 3.14`
    /// Nótese que una asignación produce como valor el
    /// valor asignado (a no ser que sea una sentencia).
    /// Es decir, `a=b=c=0` es una expresión válida que
    /// establece a,b, y c a 0.
    EXP_OP_ASIGNACION,
    /// Una definición de una función creada por el usuario.
    /// Ejemplo: `\x,y => x+y`
    EXP_OP_DEF_FUNCION,
    /// Una lista de expresiones dentro de un bloque.
    /// Ejemplo: `{ x=5; print(x); }`
    EXP_BLOQUE,
    /// Una importación de métodos de un archivo.
    /// Ejemplo: `import "math.mth"`, `import foreign "math.so"`
    EXP_IMPORT,
} TipoExpresion;

/// Una expresión de un determinado tipo.
typedef struct {
    /// El tipo de la expresión.
    TipoExpresion tipo;
    /// Si es una sentencia o no (Ej, `5` vs `5;`).
    /// Una sentencia produce como valor siempre
    /// "indefinido".
    int es_sentencia;
    union {
        Valor valor;
        Identificador identificador;
        LlamadaFuncion llamadaFuncion;
        Asignacion asignacion;
        DefinicionFuncion defFuncion;
        ListaExpresiones bloque;
        Import importe;
    };
} Expresion;

/*
 * Funciones ayuda de creación de expresiones.
 */

Expresion crear_exp_nula();
Expresion crear_exp_valor(Valor valor);
Expresion crear_exp_identificador(Identificador identificador);
Expresion crear_exp_llamada(Expresion funcion, ListaExpresiones argumentos, Localizacion loc);
Expresion crear_exp_op_unaria(Identificador operador, Expresion x, Localizacion loc);
Expresion crear_exp_op_binaria(Identificador operador, Expresion a, Expresion b, Localizacion loc);
Expresion crear_exp_asignacion(Identificador identificador, Expresion expresion, TipoAsignacion asignacion, Localizacion loc);
Expresion crear_exp_def_funcion(ListaIdentificadores argumentos, Expresion cuerpo, Localizacion loc);
Expresion crear_exp_bloque(ListaExpresiones expresiones, Localizacion loc);
Expresion crear_exp_importe(String archivo, int foraneo, Localizacion loc);

/// Crea un clon profundo de una expresión.
Expresion clonar_expresion(Expresion exp);
/// Libera la memoria de una expresión, ignorando su valor.
void borrar_expresion(Expresion *exp);

/*
 * Funciones ayuda de creación de expresiones
 */

ListaExpresiones crear_lista_expresiones();
void push_lista_expresiones(ListaExpresiones *lista, Expresion expresion);
ListaExpresiones clonar_lista_expresiones(ListaExpresiones lista);
void borrar_lista_expresiones(ListaExpresiones *lista);

/*
 * Funciones ayuda de impresión
 */

void imprimir_expresion(Expresion expresion);
void imprimir_lista_expresiones(ListaExpresiones listaExpresiones);

/*
 * Funciones adicionales
 */

/**
 * Funciones que calcula la lista de identificadores que una determinada
 * definición de función ha capturado del exterior. Estos se determinan
 * como aquellos identificadores que no son ni argumentos de la función
 * ni variables locales. Por ejemplo,
 *
 *      const five = 5
 *      \x => x+five
 *
 *  Devolvería ["five"] como lista de variables capturadas.
 *
 * @param funcion función a la que se le calcularán las variables capturadas.
 * @return lista de variables capturadas.
 */
ListaIdentificadores variables_capturadas(DefinicionFuncion funcion);

#endif //PRACTICA3_AST_H
