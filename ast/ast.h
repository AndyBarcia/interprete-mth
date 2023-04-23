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

/// Una expresión de asignación, del estilo
/// de `x = exp`, `x[exp] = exp`, `x.y = exp`
/// o cualquier combinación de accesos.
typedef struct {
    /// El nombre al que se va a asignar
    /// el valor de la expresión.
    NombreAsignable nombre;
    /// La expresión a evaluar.
    struct Expresion *expresion;
    /// La localización de la expresión
    /// en el código fuente.
    Localizacion* loc;
} ExpAsignacion;

/// Una expresión de asignación inmutable
/// del estilo de `const x = exp` o
/// `export x = exp`.
typedef struct {
    /// El nombre al que se asignará
    /// el valor de forma inmutable.
    Identificador nombre;
    /// La expresión a evaluar.
    struct Expresion *expresion;
    /// Si es de tipo `export`.
    int export;
    /// La localización de la expresión
    /// en el código fuente.
    Localizacion* loc;
} ExpDefinicion;

/// Una expresión de definición de una función,
/// del estilo de `\x => exp`.
typedef struct {
    /// Los nombres de los argumentos.
    ListaIdentificadores nombres_args;
    /// La lista de variables usadas dentro de
    /// la función que no son ni argumentos ni
    /// variables locales.
    ListaIdentificadores variables_capturadas;
    /// El cuerpo de la propia función.
    struct Expresion *cuerpo;
    /// La localización de la expresión en
    /// código fuente.
    Localizacion* loc;
} ExpFuncion;

/// Una expresión de importación, ya sea
/// del estilo de `import "fichero.mth"
/// o `import foreign "fichero.so" as nombre`
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

/// Una expresión de bucle while
/// del tipo de `while a do b`
typedef struct {
    struct Expresion* condicion;
    struct Expresion* cuerpo;
    Localizacion *loc;
} ExpWhile;

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
    /// a un nombre asignable.
    /// Ejemplo: `x = 5`, `x[2] = 3`, `x.y = 8`
    /// Nótese que una asignación produce como valor el
    /// valor asignado (a no ser que sea una sentencia).
    /// Es decir, `a=b=c=0` es una expresión válida que
    /// establece a,b, y c a 0.
    EXP_OP_ASIGNACION,
    /// Una expresión que es una asignación de un valor
    /// a un identificador de forma inmutable.
    /// Ejemplo: `const x = 5`.
    /// Se permiten expresiones del tipo `export`, que
    /// permiten que la variable sea accesible desde
    /// fuera del módulo actual.
    /// Ejemplo: `export x = 5`
    EXP_OP_DEFINICION,
    /// Una definición de una función creada por el usuario.
    /// Ejemplo: `\x,y => x+y`
    EXP_OP_FUNCION,
    /// Una lista de expresiones dentro de un bloque.
    /// Ejemplo: `{ x=5; print(x); }`
    EXP_BLOQUE,
    /// Una importación de métodos de un archivo.
    /// Ejemplo: `import "prelude.mth"`, `import foreign "math.so"`
    EXP_IMPORT,
    /// Una expresión condicional.
    /// Ejemplo: `if cond { a } else { b }`
    EXP_CONDICIONAL,
    /// Una expresión de bucle while.
    /// Ejemplo: `while cond do { a }`
    EXP_BUCLE_WHILE,
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
        ExpDefinicion definicion;
        ExpFuncion def_funcion;
        ExpBloque bloque;
        ExpImporte importe;
        ExpCondicional condicional;
        ExpWhile bucle_while;
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
Expresion crear_exp_asignacion(NombreAsignable nombre, Expresion expresion, Localizacion *loc);
Expresion crear_exp_definicion(Identificador nombre, Expresion expresion, int export, Localizacion *loc);
Expresion crear_exp_funcion(ListaIdentificadores argumentos, Expresion cuerpo, Localizacion *loc);
Expresion crear_exp_bloque(ListaExpresiones expresiones, Localizacion *loc);
Expresion crear_exp_importe(String archivo, int foraneo, Localizacion *loc);
Expresion crear_exp_importe_as(String archivo, int foraneo, Identificador as, Localizacion *loc);
Expresion crear_exp_condicional(Expresion condicion, Expresion verdadero, Expresion *falso, Localizacion *loc);
Expresion crear_exp_bucle_while(Expresion condicion, Expresion cuerpo, Localizacion *loc);
Expresion crear_exp_ctrl_flujo(TipoControlFlujo tipo, Expresion *retorno, Localizacion *loc);

Expresion crear_exp_op_unaria(FuncionIntrinseca op, Localizacion *opLoc, Expresion x, Localizacion *loc);
Expresion crear_exp_op_binaria(FuncionIntrinseca op, Localizacion *opLoc, Expresion a, Expresion b, Localizacion *loc);

/// El contexto en el que aparece una función.
/// Dependiendo del contexto en el que aparezca,
/// ciertas expresiones se convierten a errores.
typedef struct {
    int es_modulo;
    int es_funcion;
    int es_asignacion;
    int es_bucle;
    int es_bloque;
} ContextoExpresion;

/**
 * Comprueba que una expresión es válida dentro
 * de un contexto dado, y si no lo es, la convierte
 * en un error.
 * @param e expresión a validar.
 * @param contexto el contexto utilizado.
 */
void validar_expresion(Expresion *e, ContextoExpresion contexto);

/**
 * Realiza un clon profundo de una expresión.
 * @param exp la expresión a clonar.
 * @return una nueva expresión.
 */
Expresion clonar_expresion(Expresion exp);

/**
 * Libera toda la memoria asociada a una expresión.
 * @param exp la expresión a borrar.
 */
void borrar_expresion(Expresion *exp);

/**
 * Devuelve la posible localización de una expresión.
 * Se devuelve NULL si no se conoce su localización.
 * @param exp
 * @return
 */
Localizacion* obtener_loc_exp(Expresion *exp);

/**
 * Crea una nueva lista expresiones vacía, a la que se puede
 * añadir elementos con la función `push_lista_expresiones`.
 * No reserva memoria con malloc hasta que se inserta al menos
 * un valor.
 * @param loc la localización de la lista de expresiones, o NULL
 * si se desea que se calcule automáticamente en base a las
 * expresiones que se vayan añadiendo.
 * @return una nueva lista de expresiones.
 */
ListaExpresiones crear_lista_expresiones(Localizacion *loc);

/**
 * Como `crear_lista_expresiones`, pero se crea una lista con
 * un elemento inicial dado y no una lista vacía.
 * @param expresion
 * @return la lista de expresiones.
 */
ListaExpresiones crear_lista_expresiones1(Expresion expresion);

/**
 * Inserta una expresión en una lista de expresiones.
 * @param lista
 * @param expresion
 */
void push_lista_expresiones(ListaExpresiones *lista, Expresion expresion);

/**
 * Realiza un clonado profundo de una lista de expresiones.
 * @param lista
 * @return
 */
ListaExpresiones clonar_lista_expresiones(ListaExpresiones lista);

/**
 * Borra una lista de expresiones y libera toda su memoria asociada.
 * @param lista
 */
void borrar_lista_expresiones(ListaExpresiones *lista);

/**
 * Imprime una expresión por la terminal.
 * Se utiliza para debugging.
 * @param expresion
 */
void imprimir_expresion(Expresion expresion);

/**
 * Imprime una lista de expresiones por la terminal, separadas
 * por comas.
 * Se utiliza para debugging.
 * @param listaExpresiones
 */
void imprimir_lista_expresiones(ListaExpresiones listaExpresiones);

#endif //PRACTICA3_AST_H
