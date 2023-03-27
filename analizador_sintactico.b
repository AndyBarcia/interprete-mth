%{
#include "definiciones.h"
#include "analizador_lexico.h"
#include "string.h"

void yyerror(const char* s);
%}

%union {
    int tipoOperador;
    int valorEntero;
    String error;
    String identificador;
}

%token <valorEntero> ENTERO
%token <identificador> IDENTIFICADOR
%token <error> ERROR

%token OPERADOR_BINARIO
%token OPERADOR_UNARIO
%token OPERADOR_ASIGNACION

%token PARENTESIS_IZQ
%token PARENTESIS_DER
%token CORCHETE_IZQ
%token CORCHETE_DER
%token LLAVE_IZQ
%token LLAVE_DER

%token COMA

%%

program:
    statement_list
    ;

statement_list:
    statement { printf("> "); }
    | statement_list statement { printf("> "); }
    ;

argument_list:
    expresion
    | argument_list expresion
    ;

expresion:
    IDENTIFICADOR { printf("Identificador: %s\n", string_a_puntero(&$1)); }
    | ENTERO { printf("Entero: %d\n", $1); }
    | OPERADOR_UNARIO expresion
    | expresion OPERADOR_BINARIO expresion
    | IDENTIFICADOR PARENTESIS_IZQ argument_list PARENTESIS_DER { printf("%d", $3); }
    | ERROR
    ;

asignacion:
    IDENTIFICADOR OPERADOR_ASIGNACION expresion
    ;

statement:
    expresion
    | asignacion
    ;

%%

void yyerror(const char* s) {
    printf("Error: %s\n", s);
}

int yywrap() {
    return 1;
}
