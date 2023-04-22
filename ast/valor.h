#ifndef PRACTICA3_VALOR_H
#define PRACTICA3_VALOR_H

#include "../string.h"
#include "../error.h"
#include "../bibliotecas/biblioteca_dinamica.h"
#include "../analizador_lexico.h"

#include "identificador.h"

/// Todos los tipos de valores que hay en el lenguaje
typedef enum {
    /// Valor de tipo indefinido.
    /// Es un tipo especial que se usa internamente en
    /// algunos lugares del evaluador.
    TIPO_INDEFINIDO,
    /// Valor de un tipo unidad.
    /// Todas las sentencias son de tipo unidad.
    TIPO_UNIDAD,
    /// Valor que representa un error léxico, de
    /// sintaxis, o un error en tiempo de ejecución.
    TIPO_ERROR,
    /// Valor entero. Ejemplo: `5`
    TIPO_ENTERO,
    /// Valor decimal. Ejemplo: `5.5`
    TIPO_DECIMAL,
    /// Valor booleano. Ejemplo: `verdadero`
    TIPO_BOOL,
    /// Valor string. Ejemplo `"string"`
    TIPO_STRING,
    /// Valor de función intrínseca. Ejemplo `(+)`
    TIPO_FUNCION_INTRINSECA,
    /// Valor de puntero a función de C.
    TIPO_FUNCION_FORANEA,
    /// Valor de función definida por el usuario.
    TIPO_FUNCION,
    /// Una biblioteca de C
    TIPO_BIBLIOTECA_FORANEA,
    /// Un valor resultando de una expresión de control 
    /// de flujo, como un `return` o un `break`.
    TIPO_CONTROL_FLUJO
} TipoValor;

/// Tipo interno de un entero en el lenguaje
typedef int Entero;
/// Tipo interno de un bool en el lenguaje
typedef int Bool;
/// Tipo entero de un número decimal.
typedef double Decimal;

/// Las distintas funciones intrínsecas del lenguaje.
typedef enum {
    /// Operación (+) de suma de enteros y decimales
    /// y concatenación de strings.
    INTRINSECA_SUMA,
    /// Operación de resta (-) de enteros y decimales.
    INTRINSECA_RESTA,
    /// Operación de multiplicación (*) de enteros y decimales.
    /// También permite concatenación repetida de strings.
    INTRINSECA_MULT,
    /// Operación de división (/) decimal de enteros y decimales.
    /// Siempre devuelve un decimal aunque se dividan dos enteros
    /// y el resultado esa exacto.
    INTRINSECA_DIV,
    /// Operación de módulo (%) para enteros y decimales.
    INTRINSECA_MOD,
    /// Operación de igualdad (==) o `equal`
    INTRINSECA_EQ,
    /// Operación de desigualdad (!=) o `not equal`
    INTRINSECA_NEQ,
    /// Operación de comparación (>) o `greater`
    INTRINSECA_GE,
    /// Operación de comparación (>=) o `greater or equal`
    INTRINSECA_GEQ,
    /// Operación de comparación (<) o `lesser`
    INTRINSECA_LE,
    /// Operación de comparación (<=) o `lesser or equal`
    INTRINSECA_LEQ,
    /// Operación lógica AND
    INTRINSECA_AND,
    /// Operación lógica OR
    INTRINSECA_OR,
    /// Operación lógica NOT
    INTRINSECA_NOT,
    /// Operación de negación (-) de enteros y decimales.
    INTRINSECA_NEGAR,
    /// Imprimir uno o varios valores.
    INTRINSECA_PRINT,
    /// Imprimir el espacio de trabajo.
    INTRINSECA_PRINTWS,
    /// Resetear el espacio de trabajo
    INTRINSECA_RESETWS,
    /// Ayuda sobre un determinado valor.
    INTRINSECA_AYUDA,
    /// Cargar un fichero como un string.
    INTRINSECA_CARGAR,
    /// Evalua un string como si se hubiese
    /// ejecutado en la terminal.
    INTRINSECA_EVAL,
    /// Termina la ejecución del programa.
    INTRINSECA_EXIT,
    /// Llama a una función de una biblioteca
    /// externa de C.
    INTRINSECA_CALLFOREIGN,
    /// Función de casting a enteros.
    /// Es un casting estricto, por lo que sólo
    /// decimales se pueden convertir a enteros.
    INTRINSECA_CAST_ENTERO,
    /// Función de casting a decimales.
    /// Es un casting estricto, por lo que sólo
    /// enteros se pueden convertir a decimales.
    INTRINSECA_CAST_DECIMAL,
} FuncionIntrinseca;

/// Una función definida por el usuario, resultado
/// de una expresión de definición de función del
/// estilo de \x,y => x+y
typedef struct {
	/// Una serie de nombres como argumentos de entrada.
    ListaIdentificadores nombres_args;
    /// Un conjunto de valores capturados de su entorno.
    struct TablaHash *variables_capturadas;
    /// El cuerpo de la función en sí.
    struct Expresion *cuerpo;
} Funcion;

/// Un valor resultante de una expresión de control
/// de flujo del estilo de `return` o `break`.
typedef struct {
    TipoControlFlujo tipo;
    struct Valor *valor;
} ValorControlFlujo;

/// Un valor del lenguaje de programación.
/// Los valores dinámicos (strings, errores, y funciones) llevan
/// una cuenta de referencias activas para así evitar double free's
/// y evitar mallocs innecesarios.
typedef struct {
    //// El tipo de valor.
    TipoValor tipo_valor;
    /// El número de referencias dinámicas,
    /// o NULL en caso de valores triviales.
    int *referencias;
    /// La localización en el código fuente, si existe.
    Localizacion *loc;
    union {
        Entero entero;
        Decimal decimal;
        Bool bool;
        String string;
        FuncionIntrinseca funcion_intrinseca;
        Funcion funcion;
        FuncionForanea funcion_foranea;
        BibilotecaDinamica biblioteca;
        ValorControlFlujo control_flujo;
        Error error;
    };
} Valor;

/// Una lista de valores del lenguaje de programación.
typedef struct {
    int capacidad;
    /// La cantidad de valores de la lista.
    int longitud;
    /// La lista de valores.
    Valor* valores;
    /// La localización en el código fuente, si existe.
    Localizacion *loc;
} ListaValores;

/*
 * Funciones ayuda de creación de valores.
 */

Valor crear_valor_unidad(Localizacion *loc);
Valor crear_valor_indefinido();
Valor crear_entero(Entero entero, Localizacion *loc);
Valor crear_decimal(Decimal decimal, Localizacion *loc);
Valor crear_bool(Bool bool, Localizacion *loc);
Valor crear_valor_string(String string, Localizacion *loc);
Valor crear_funcion_intrinseca(FuncionIntrinseca funcion, Localizacion *loc);
Valor crear_funcion(ListaIdentificadores argumentos, struct Expresion *cuerpo, struct TablaHash *capturadas, Localizacion *loc);
Valor crear_funcion_foranea(FuncionForanea foranea);
Valor crear_valor_biblioteca(BibilotecaDinamica biblioteca, Localizacion *loc);
Valor crear_valor_control_flujo(TipoControlFlujo tipo, Valor *v, Localizacion *loc);
Valor crear_valor_error(Error error, Localizacion *loc);

/**
 * Extra el valor asociado a un elemento de control de flujo.
 * Por ejemplo, dado el valor `return 5`, se extrae el valor `5`.
 * En caso de que no haya ningún valor asociado, se devuelve
 * el valor unidad.
 * @param control_flujo
 * @return
 */
Valor extraer_valor_control_flujo(ValorControlFlujo *control_flujo);

/**
 * Crea un clon ligero. Esto simplemente aumenta el número de referencias
 * dinámicas de los valores con memora dinámica (strings, errores, funciones,
 * etc), por lo que es bastante barato de llamar.
 * @param v el valor clonado.
 * @return
 */
Valor clonar_valor(Valor v);

/**
 * Crea un clon débil, que no aumenta el número de referencias dinámicas
 * incluso aunque sea de tipo dinámico.
 * Un clon débil puede apuntar a memoria inválida ya liberada si el valor
 * subyacente clonado se libera.
 * @param v
 * @return
 */
Valor clonar_valor_debil(Valor v);

/**
 * Borra la memoria dinámica de un valor. Esto implica borrar la información
 * de localización si la hay, y reducir el número de referencias dinámicas
 * en caso de valores con memoria dinámica.
 *
 * En los valores con memoria dinámica sólo se borra la memoria subyacente
 * si el número de referencias dinámicas llega a 0.
 * @param valor
 */
void borrar_valor(Valor *valor);

/**
 * Compara dos valores, guardando el resultado de la comparación en un
 * puntero a entero. El resultado de la función indica si tiene sentido
 * que se realice la comparación.
 * @param a el primer valor.
 * @param b el segundo valor.
 * @param resultado pasa a ser 0 si a y b son iguales; 1 si a es mayor que
 * b, y -1 si a es menor que b. Su valor no varía si la comparación no tiene
 * sentido.
 * @return devuelve 1 si tiene sentido realizar la comparación y 0 en caso
 * contrario. Hay ciertos valores, como las funciones, que no tiene sentido
 * comparar, por lo que se devuelve 0.
 * En general también se devuelve 0 si se están comparando valores de distintos
 * tipos, ya que no tiene sentido decir cuál es mayor o menor (excepto, por
 * ejemplo, con enteros y decimales).
 */
int comparar_valor(Valor a, Valor b, int *resultado);

/**
 * Trata de acceder a un miembro de un valor con un índice dado. Con esto
 * se pueden hacer cosas como acceder a un índice de un array; acceder
 * a una función foránea de una biblioteca con un string, etc.
 * @param valor el valor que se va a indexar
 * @param indice el valor que se usará como índice
 * @param resultado donde se pondrá el resultado.
 * @return devuelve 1 si tiene sentido la operación, 0 en caso de que
 * no se pueda realizar el indexado con los tipos de valores usados.
 */
int acceder_miembro_valor(Valor valor, Valor indice, Valor *resultado);

/**
 * Crea una nueva lista vacía de valores, a la que se puede
 * añadir elementos con la función `push_lista_valores`.
 * No reserva memoria con malloc hasta que se inserta al menos
 * un valor.
 * @return una nueva lista de valores.
 */
ListaValores crear_lista_valores();

/**
 * Añade un valor a una lista de valores dada.
 * La memoria del valor insertado pasa a ser manejada por la
 * lista, por lo que no se debe liberar la memoria del valor
 * manualmente.
 * @param lista la lista en la que se insertará el valor.
 * @param v el valor insertado.
 */
void push_lista_valores(ListaValores *lista, Valor v);

/**
 * Borra una lista de valores y libera la memoria de todos
 * los valores de la lista.
 * @param lista
 */
void borrar_lista_valores(ListaValores *lista);

/**
 * Imprime un valor por consola, con
 * un salto de línea final.
 * @param valor el valor a imprimir
 */
void imprimir_valor(Valor valor);

/**
 * Convierte un tipo de valor a una representación en forma
 * de string, como "entero" o "decimal".
 * @param tipo el tipo a convertir.
 * @return la cadena estática.
 */
char* tipo_valor_a_str(TipoValor tipo);

#endif //PRACTICA3_VALOR_H
