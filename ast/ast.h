#ifndef PRACTICA3_AST_H
#define PRACTICA3_AST_H

#include "../string.h"
#include "valor.h"
#include "../analizador_lexico.h"
#include "../tabla_simbolos.h"
#include "control_flujo.h"
#include "nombre_asignable.h"

/// Una expresión que contiene un valor.
typedef Valor ExpValor;

/// Una expresión que es un nombre,
/// del estilo de `x`, `x.y`, `x[y]`
/// y combinaciones de ellos.
typedef NombreAsignable ExpNombre;

/// Una lista de expresiones, con una
/// determinada localización en el
/// código fuente.
typedef struct {
    int capacidad;
    int longitud;
    struct Expresion* valores;
    Localizacion* loc;
} ListaExpresiones;

/// Expresión de llamada a una función,
/// del estilo de `exp(args)`.
typedef struct {
    struct Expresion *funcion;
    ListaExpresiones args;
    Localizacion* loc;
} ExpLlamada;

/// Una expresión de asignación, del
/// estilo de `x = exp`.
typedef struct {
    /// El nombre al que se va a asignar
    /// el valor de la expresión.
    NombreAsignable nombre;
    /// La expresión a evaluar.
    struct Expresion *expresion;
    /// Si la asignación es normal, o
    /// si es de tipo inmutable o export.
    /// Ejemplo: `const x = 5`
    /// Ejemplo: `export pi = 3.14`
    TipoAsignacion tipo;
    /// La localización de la expresión
    /// en el código fuente.
    Localizacion* loc;
} ExpAsignacion;

/// Una expresión de definición de una función,
/// del estilo de `\x => exp`.
typedef struct {
    /// Los nombres de los argumentos.
    ListaIdentificadores nombres_args;
    /// El cuerpo de la propia función.
    struct Expresion *cuerpo;
    /// La localización de la expresión en
    /// código fuente.
    Localizacion* loc;
} ExpDefFuncion;

/// Una expresión de importación, ya sea
/// del estilo de `import "fichero.mth"
/// o `import foreign "fichero.so" as nombre.
typedef struct {
    /// El archivo o biblioteca a cargar.
    String archivo;
    /// Si es una biblioteca foránea.
    int foraneo;
    /// El identificador con el que se
    /// va a cargar la biblioteca.
    Identificador *as;
    /// La localización en el código fuente.
    Localizacion* loc;
} ExpImporte;

/// Una lista de expresiones en un
/// bloque de tipo `{ [exp] }`.
typedef struct {
    ListaExpresiones lista;
    Localizacion *loc;
} ExpBloque;

/// Una expresión condicional del
/// tipo `if a then b else c`.
typedef struct {
    struct Expresion* condicion;
    struct Expresion* verdadero;
    struct Expresion* falso;
    Localizacion *loc;
} ExpCondicional;

/// Una expresión de control de flujo, como
/// un `return` o un `break`.
/// Tiene un posible valor de retorno asociado,
/// como con `return 5`.
typedef struct {
    /// El tipo de control de flujo.
    TipoControlFlujo tipo;
    /// El opcional valor de retorno.
    struct Expresion* retorno;
    /// La localización en el código.
    Localizacion* loc;
} ExpControlFlujo;

/// El tipo de una expresión.
typedef enum {
    /// Una expresión desconocida.
    /// Sólo se utiliza internamente en el código.
    EXP_NULA,
    /// Una expresión que es un simple valor.
    /// Ejemplo: `5`
    EXP_VALOR,
    /// Una expresión que es un nombre que designa una
    /// variable o lugar.
    /// Ejemplo: `x`, `x.y`, `x[y]`, etc.
    EXP_NOMBRE,
    /// Una expresión que es una llamada a una función
    /// con ciertos nombres_args.
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
    /// Una expresión condicional.
    /// Ejemplo: `if cond { a } else { b }`
    EXP_CONDICIONAL,
    /// Un `return` o un `break`.
    EXP_CONTROL_FLUJO,
} TipoExpresion;

/// Una expresión de un determinado tipo.
typedef struct {
    /// El tipo de la expresión.
    TipoExpresion tipo;
    /// Si es una sentencia o no (Ej, `5` vs `5;`).
    /// Una sentencia produce como valor siempre
    /// "indefinido", excepto en las expresiones
    /// de control de flujo.
    int es_sentencia;
    union {
        ExpValor valor;
        ExpNombre nombre;
        ExpLlamada llamada_funcion;
        ExpAsignacion asignacion;
        ExpDefFuncion def_funcion;
        ExpBloque bloque;
        ExpImporte importe;
        ExpCondicional condicional;
        ExpControlFlujo control_flujo;
    };
} Expresion;

/*
 * Funciones ayuda de creación de expresiones.
 */

Expresion crear_exp_nula();
Expresion crear_exp_valor(Valor valor);
Expresion crear_exp_nombre(NombreAsignable nombre);
//Expresion crear_exp_acceso(Expresion valor, Acceso acceso, Localizacion *loc);
Expresion crear_exp_llamada(Expresion funcion, ListaExpresiones argumentos, Localizacion *loc);
Expresion crear_exp_asignacion(NombreAsignable nombre, Expresion expresion, TipoAsignacion asignacion, Localizacion *loc);
Expresion crear_exp_def_funcion(ListaIdentificadores argumentos, Expresion cuerpo, Localizacion *loc);
Expresion crear_exp_bloque(ListaExpresiones expresiones, Localizacion *loc);
Expresion crear_exp_importe(String archivo, int foraneo, Localizacion *loc);
Expresion crear_exp_importe_as(String archivo, int foraneo, Identificador as, Localizacion *loc);
Expresion crear_exp_condicional(Expresion condicion, Expresion verdadero, Expresion *falso, Localizacion *loc);
Expresion crear_exp_ctrl_flujo(TipoControlFlujo tipo, Expresion *retorno, Localizacion *loc);

Expresion crear_exp_op_unaria(FuncionIntrinseca op, Localizacion *opLoc, Expresion x, Localizacion *loc);
Expresion crear_exp_op_binaria(FuncionIntrinseca op, Localizacion *opLoc, Expresion a, Expresion b, Localizacion *loc);

/// Crea un clon profundo de una expresión.
Expresion clonar_expresion(Expresion exp);
/// Libera la memoria de una expresión, ignorando su valor.
void borrar_expresion(Expresion *exp);

/// Devuelve la posible localización de una expresión.
/// Se devuelve NULL si no se conoce su localización.
Localizacion* obtener_loc_exp(Expresion *exp);

/*
 * Funciones ayuda de creación de expresiones
 */

ListaExpresiones crear_lista_expresiones(Localizacion *loc);
ListaExpresiones crear_lista_expresiones1(Expresion expresion);
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
 * como aquellos identificadores que no son ni nombres_args de la función
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
ListaIdentificadores variables_capturadas(ExpDefFuncion funcion);

#endif //PRACTICA3_AST_H
