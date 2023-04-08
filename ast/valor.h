#ifndef PRACTICA3_VALOR_H
#define PRACTICA3_VALOR_H

#include "../string.h"
#include "../error.h"
#include "../biblioteca_dinamica.h"
#include "../analizador_lexico.h"

#include "identificador.h"

/// Tipo interno de un entero en el lenguaje
typedef int Entero;
/// Tipo interno de un bool en el lenguaje
typedef int Bool;

/// Todos los tipos de valores que hay en el lenguaje
typedef enum {
    /// Valor de tipo nulo.
    /// Sólo se utiliza internamente en el código.
    TIPO_NULO,
    /// Valor de un tipo no especificado.
    /// Todas las sentencias son de tipo indefinido.
    TIPO_INDEFINIDO,
    /// Valor que representa un error léxico, de
    /// sintaxis, o un error en tiempo de ejecución.
    TIPO_ERROR,
    /// Valor entero. Ejemplo: `5`
    TIPO_ENTERO,
    /// Valor booleano. Ejemplo: `verdadero`
    TIPO_BOOL,
    /// Valor string. Ejemplo `"string"`
    TIPO_STRING,
    /// Valor de puntero a función nativa. Ejemplo `+`
    TIPO_FUNCION_INTRINSECA,
    /// Valor de puntero a función de C.
    TIPO_FUNCION_FORANEA,
    /// Valor de función definida por el usuario.
    TIPO_FUNCION,
    /// Una biblioteca de C
    TIPO_BIBLIOTECA_FORANEA
} TipoValor;

typedef void (*FuncionIntrinseca)();

/// Un valor asignable en una expresión de asignación.
typedef Identificador NombreAsignable;

/// Una función definida por el usuario, con:
///     * Una serie de nombres como nombres_args de entrada.
///     * Un conjunto de variables capturadas de su entorno.
///     * El cuerpo de la función en sí.
typedef struct {
    ListaIdentificadores nombres_args;
    struct TablaHash *variables_capturadas;
    struct Expresion *cuerpo;
} Funcion;

/// Un valor del lenguaje de programación.
/// Los valores dinámicos (strings, errores, y funciones) llevan
/// una cuenta de referencias activas para así evitar double free's.
typedef struct {
    //// El tipo de valor.
    TipoValor tipo_valor;
    /// El número de referencias dinámicas.
    int *referencias;
    /// La localización en el código fuente, si existe.
    Localizacion *loc;
    union {
        Entero entero;
        Bool bool;
        String string;
        FuncionIntrinseca funcion_intrinseca;
        Funcion funcion;
        FuncionForanea funcion_foranea;
        BibilotecaDinamica biblioteca;
        Error error;
    };
} Valor;

/// Una lista de valores del lenguaje de programación.
typedef struct {
    int longitud;
    struct Valor* valores;
} ListaValores;

/*
 * Funciones ayuda de creación de valores.
 */

Valor crear_indefinido();
Valor crear_nulo();
Valor crear_entero(Entero entero, Localizacion *loc);
Valor crear_bool(Bool bool, Localizacion *loc);
Valor crear_valor_string(String string, Localizacion *loc);
Valor crear_funcion_nativa(FuncionIntrinseca funcion, Localizacion *loc);
Valor crear_funcion(ListaIdentificadores argumentos, struct Expresion *cuerpo, struct TablaHash *capturadas, Localizacion *loc);
Valor crear_funcion_foranea(FuncionForanea foranea);
Valor crear_valor_biblioteca(BibilotecaDinamica biblioteca, Localizacion *loc);
Valor crear_valor_error(Error error, Localizacion *loc);

/// Crea un clon "ligero" de un valor; esto es, incrementando
/// la cuenta de referencias dinámicas del valor.
Valor clonar_valor(Valor v);
/// Borra un valor; reduciendo su cuenta de referencias dinámicas,
/// y liberando su memoria en caso de llegar a 0.
void borrar_valor(Valor *valor);

/// Devuelve 1 si dos valores son iguales, y 0 en caso contrario.
int comparar_valor(Valor a, Valor b);

void borrar_lista_valores(ListaValores *lista);

/*
 * Funciones ayuda de impresión
 */

void _imprimir_valor(Valor valor);
void imprimir_valor(Valor valor);

#endif //PRACTICA3_VALOR_H
