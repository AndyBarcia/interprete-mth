#ifndef PRACTICA3_AST_H
#define PRACTICA3_AST_H

#include "string.h"

typedef int Entero;

#define TIPO_INDEFINIDO 0
#define TIPO_NULO 1
#define TIPO_ERROR 2
#define TIPO_ENTERO 3
#define TIPO_FUNCION_NATIVA 4
#define TIPO_FUNCION 5

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
    struct Expresion *cuerpo;
} Funcion;

typedef struct {
    int tipoValor;
    union {
        Entero entero;
        FuncionNativa funcion_nativa;
        Funcion funcion;
        String error;
    };
} Valor;

Valor crear_indefinido();
Valor crear_nulo();
Valor crear_entero(int entero);
Valor crear_funcion_nativa(FuncionNativa funcion);

Valor crear_error(String error);
void imprimir_valor(Valor valor);

typedef struct {
    int operador;
    struct Expresion *valor;
} OperacionUnaria;

typedef struct {
    int operador;
    struct Expresion *izq;
    struct Expresion *der;
} OperacionBinaria;

typedef struct {
    String identificador;
    struct Expresion *expresion;
} Asignacion;

typedef struct {
    String identificador_funcion;
    ListaExpresiones argumentos;
} LlamadaFuncion;

#define EXP_VALOR 0
#define EXP_IDENTIFICADOR 1
#define EXP_OP_UNARIA 2
#define EXP_OP_BINARIA 3
#define EXP_OP_LLAMADA 4
#define EXP_OP_ASIGNACION 5
#define EXP_BLOQUE 6
#define EXP_DEF_FUNCION 7

typedef struct {
    int tipo;
    union {
        Valor valor;
        String identificador;
        OperacionUnaria operacionUnaria;
        OperacionBinaria  operacionBinaria;
        LlamadaFuncion llamadaFuncion;
        Asignacion asignacion;
        ListaExpresiones bloque;
    };
} Expresion;

Valor crear_funcion(ListaIdentificadores argumentos, Expresion cuerpo);

void imprimir_expresion(Expresion expresion);

Expresion crear_exp_valor(Valor valor);
Expresion crear_exp_identificador(String identificador);
Expresion crear_exp_op_unaria(int op, Expresion  exp);
Expresion crear_exp_op_binaria(Expresion expA, int op, Expresion expB);
Expresion crear_exp_llamada(String identificador, ListaExpresiones argumentos);
Expresion crear_exp_asignacion(String identificador, Expresion expresion);
Expresion crear_exp_bloque(ListaExpresiones expresiones);

ListaExpresiones crear_lista_expresiones();
void push_lista_expresiones(ListaExpresiones *lista, Expresion expresion);
void imprimir_lista_expresiones(ListaExpresiones listaValores);

#endif //PRACTICA3_AST_H
