#ifndef LEXER_ANALIZADOR_LEXICO_H
#define LEXER_ANALIZADOR_LEXICO_H

#include "tabla_simbolos.h"
#include "analizador_sintactico.h"

typedef struct {
    // El escáner de bison
    void* scanner;
    // Buffer que se utiliza si
    // estamos procesando un char*
    // en vez de un archivo.
    void* str_buffer;
} Lexer;

Lexer crear_lexer_fichero(FILE *fichero);

Lexer crear_lexer_str(char *str);

void borrar_analizador_lexico(Lexer lexer);

int siguiente_componente_lexico(Lexer lexer, YYSTYPE *yylval_param);

#endif //LEXER_ANALIZADOR_LEXICO_H
