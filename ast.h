#ifndef PRACTICA3_AST_H
#define PRACTICA3_AST_H

#include "string.h"

typedef int Entero;
typedef int Bool;

#define TIPO_INDEFINIDO 0
#define TIPO_NULO 1
#define TIPO_ERROR 2
#define TIPO_ENTERO 3
#define TIPO_BOOL 4
#define TIPO_STRING 5
#define TIPO_FUNCION_NATIVA 6
#define TIPO_FUNCION 7

typedef struct {
    int longitud;
    struct Valor* valores;
} ListaValores;

typedef void (*FuncionNativa)();

typedef struct {
    int longitud;
    int capacidad;
    String* valores;
} ListaIdentificadores;

ListaIdentificadores crear_lista_identificadores();
void push_lista_identificadores(ListaIdentificadores *lista, String identificador);
ListaIdentificadores clonar_lista_identificadores(ListaIdentificadores lista);
void borrar_lista_identificadores(ListaIdentificadores *lista);

typedef struct {
    int capacidad;
    int longitud;
    struct Expresion* valores;
} ListaExpresiones;

typedef struct {
    ListaIdentificadores argumentos;
    struct TablaHash *variables_capturadas;
    struct Expresion *cuerpo;
} Funcion;

typedef struct {
    int tipoValor;
    int *referencias;
    union {
        Entero entero;
        Bool bool;
        FuncionNativa funcion_nativa;
        Funcion funcion;
        String string;
        String error;
    };
} Valor;

Valor crear_indefinido();
Valor crear_nulo();
Valor crear_entero(Entero entero);
Valor crear_bool(Bool bool);
Valor crear_valor_string(String string);
Valor crear_funcion_nativa(FuncionNativa funcion);

Valor clonar_valor(Valor v);

void borrar_valor(Valor *valor);
void borrar_lista_valores(ListaValores *lista);

Valor crear_error(const char *formato, ...);
void imprimir_valor(Valor valor);
int comparar_valor(Valor a, Valor b);

typedef struct {
    struct Expresion *funcion;
    ListaExpresiones argumentos;
} LlamadaFuncion;

typedef struct {
    String identificador;
    struct Expresion *expresion;
    int inmutable;
} Asignacion;

typedef struct {
    ListaIdentificadores argumentos;
    struct Expresion *cuerpo;
} DefinicionFuncion;

ListaIdentificadores variables_capturadas(DefinicionFuncion funcion);

#define EXP_NULA (-1)
#define EXP_VALOR 0
#define EXP_IDENTIFICADOR 1
#define EXP_OP_LLAMADA 2
#define EXP_OP_ASIGNACION 3
#define EXP_OP_DEF_FUNCION 4
#define EXP_BLOQUE 5

typedef struct {
    int tipo;
    int es_sentencia;
    union {
        Valor valor;
        String identificador;
        LlamadaFuncion llamadaFuncion;
        Asignacion asignacion;
        DefinicionFuncion defFuncion;
        ListaExpresiones bloque;
    };
} Expresion;

Valor crear_funcion(ListaIdentificadores argumentos, Expresion *cuerpo, struct TablaHash *capturadas);

Expresion crear_exp_nula();
Expresion crear_exp_valor(Valor valor);
Expresion crear_exp_identificador(String identificador);
Expresion crear_exp_llamada(Expresion funcion, ListaExpresiones argumentos);
Expresion crear_exp_op_unaria(String operador, Expresion x);
Expresion crear_exp_op_binaria(String operador, Expresion a, Expresion b);
Expresion crear_exp_asignacion(String identificador, Expresion expresion, int inmutable);
Expresion crear_exp_def_funcion(ListaIdentificadores argumentos, Expresion cuerpo);
Expresion crear_exp_bloque(ListaExpresiones expresiones);

Expresion clonar_expresion(Expresion exp);
void borrar_expresion(Expresion *exp);

ListaExpresiones crear_lista_expresiones();
void push_lista_expresiones(ListaExpresiones *lista, Expresion expresion);
ListaExpresiones clonar_lista_expresiones(ListaExpresiones lista);
void borrar_lista_expresiones(ListaExpresiones *lista);

void imprimir_expresion(Expresion expresion);
void imprimir_lista_expresiones(ListaExpresiones listaExpresiones);
void imprimir_lista_identificadores(ListaIdentificadores listaIdentificadores);

#endif //PRACTICA3_AST_H
