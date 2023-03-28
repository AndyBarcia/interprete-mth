#ifndef PRACTICA3_EVALUADOR_H
#define PRACTICA3_EVALUADOR_H

#include "ast.h"
#include "tabla_simbolos.h"

Valor evaluar_expresion(TablaSimbolos *tabla, Expresion exp);

ListaValores evaluar_expresiones(TablaSimbolos *tabla, ListaExpresiones listaExpresiones);

#endif //PRACTICA3_EVALUADOR_H
