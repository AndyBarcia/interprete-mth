#ifndef PRACTICA3_EVALUAR_H
#define PRACTICA3_EVALUAR_H

#include "string.h"

typedef int Entero;

#define TIPO_NULO 0
#define TIPO_ERROR 1
#define TIPO_ENTERO 2
#define TIPO_IDENTIFICADOR 3
#define TIPO_FUNCION 4

struct Valor;
struct ListaValores;

typedef struct {
    int tipoValor;
    union {
        Entero entero;
        String error;
        String identificador;
    };
} Valor;

Valor crear_entero(int entero);
Valor crear_identificador(String identificador);
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
    String identificador_funcion;
    struct ListaExpresiones *argumentos;
} LlamadaFuncion;

#define EXP_VALOR 0
#define EXP_OP_UNARIA 1
#define EXP_OP_BINARIA 2
#define EXP_OP_LLAMADA 3

typedef struct {
    int tipo;
    union {
        Valor valor;
        OperacionUnaria operacionUnaria;
        OperacionBinaria  operacionBinaria;
        LlamadaFuncion llamadaFuncion;
    };
} Expresion;

void imprimir_expresion(Expresion expresion);

typedef struct {
    int capacidad;
    int longitud;
    Expresion* valores;
} ListaExpresiones;

Expresion crear_exp_valor(Valor valor);
Expresion crear_exp_op_unaria(int op, Expresion  exp);
Expresion crear_exp_op_binaria(Expresion expA, int op, Expresion expB);
Expresion crear_exp_llamada(String identificador, ListaExpresiones argumentos);

#define ENUNCIADO_EXPRESION 0
#define ENUNCIADO_ASIGNACION 0

typedef struct {
    String identificador;
    Expresion valor;
} Asignacion;

typedef struct {
    int tipo;
    union {
        Expresion expresion;
        Asignacion asignacion;
    };
} Enunciado;

Enunciado crear_enunciado_exp(Expresion expresion);
Enunciado crear_enunciado_asignacion(String identificador, Expresion expresion);

typedef struct {
    int capacidad;
    int longitud;
    Valor* valores;
} ListaIdentificadores;

typedef struct {
    ListaIdentificadores argumentos;

} Funcion;

ListaExpresiones crear_lista_expresiones();
void push_lista_expresiones(ListaExpresiones *lista, Expresion expresion);
void imprimir_lista_expresiones(ListaExpresiones listaValores);

#endif //PRACTICA3_EVALUAR_H
