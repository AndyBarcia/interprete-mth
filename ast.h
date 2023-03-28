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
#define TIPO_FUNCION_NATIVA 5
#define TIPO_FUNCION 6

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

typedef struct {
    int capacidad;
    int longitud;
    struct Expresion* valores;
} ListaExpresiones;

typedef struct {
    ListaIdentificadores argumentos;
    //TablaHash variables_capturadas;
    struct Expresion *cuerpo;
} Funcion;

typedef struct {
    int tipoValor;
    union {
        Entero entero;
        Bool bool;
        FuncionNativa funcion_nativa;
        Funcion funcion;
        String error;
    };
} Valor;

Valor crear_indefinido();
Valor crear_nulo();
Valor crear_entero(Entero entero);
Valor crear_bool(Bool bool);
Valor crear_funcion_nativa(FuncionNativa funcion);

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

Valor crear_funcion(ListaIdentificadores argumentos, Expresion cuerpo);

Expresion crear_exp_valor(Valor valor);
Expresion crear_exp_identificador(String identificador);
Expresion crear_exp_llamada(Expresion funcion, ListaExpresiones argumentos);
Expresion crear_exp_asignacion(String identificador, Expresion expresion, int inmutable);
Expresion crear_exp_def_funcion(ListaIdentificadores argumentos, Expresion cuerpo);
Expresion crear_exp_bloque(ListaExpresiones expresiones);

ListaExpresiones crear_lista_expresiones();
void push_lista_expresiones(ListaExpresiones *lista, Expresion expresion);

void imprimir_expresion(Expresion expresion);
void imprimir_lista_expresiones(ListaExpresiones listaValores);
void imprimir_lista_identificadores(ListaIdentificadores listaIdentificadores);

#endif //PRACTICA3_AST_H
