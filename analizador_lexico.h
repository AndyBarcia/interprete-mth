#ifndef LEXER_ANALIZADOR_LEXICO_H
#define LEXER_ANALIZADOR_LEXICO_H

#include "tabla_simbolos.h"
#include "analizador_sintactico.h"

typedef void* Lexer;

Lexer crear_analizador_lexico(FILE *fichero);

void borrar_analizador_lexico(Lexer lexer);

int siguiente_componente_lexico(Lexer lexer, YYSTYPE *yylval_param);

#endif //LEXER_ANALIZADOR_LEXICO_H
