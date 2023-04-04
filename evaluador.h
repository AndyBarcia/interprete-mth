#ifndef PRACTICA3_EVALUADOR_H
#define PRACTICA3_EVALUADOR_H

#include "ast.h"
#include "tabla_simbolos.h"

Valor evaluar_fichero(TablaSimbolos *tabla, FILE *entrada);

Valor evaluar_archivo(TablaSimbolos *tabla, char* archivo);

Valor evaluar_str(TablaSimbolos *tabla, char* str);

Valor evaluar_expresion(TablaSimbolos *tabla, Expresion *exp);

ListaValores evaluar_expresiones(TablaSimbolos *tabla, ListaExpresiones *listaExpresiones);

#endif //PRACTICA3_EVALUADOR_H
