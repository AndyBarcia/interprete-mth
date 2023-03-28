#ifndef LEXER_ANALIZADOR_LEXICO_H
#define LEXER_ANALIZADOR_LEXICO_H

#include "tabla_simbolos.h"

void establecer_fichero_entrada(FILE *fichero);

void inicializar_analizador_lexico();

void borrar_analizador_lexico();

int yylex();

#endif //LEXER_ANALIZADOR_LEXICO_H
