%{
#include "definiciones.h"
#include "analizador_lexico.h"
#include "string.h"

void yyerror(const char* s);
%}

%code requires {
    #include "evaluar.h"
}

%union {
    int tipoOperador;
    int valorEntero;
    String error;
    String identificador;
    ListaExpresiones listaValores;
    Valor valor;
    Expresion expresion;
    Enunciado enunciado;
}

%token <valorEntero> ENTERO
%token <identificador> IDENTIFICADOR
%token <error> ERROR

%token <tipoOperador> OPERADOR_BINARIO
%token <tipoOperador> OPERADOR_UNARIO
%token <tipoOperador> OPERADOR_ASIGNACION

%token PARENTESIS_IZQ
%token PARENTESIS_DER
%token CORCHETE_IZQ
%token CORCHETE_DER
%token LLAVE_IZQ
%token LLAVE_DER

%token FLECHA
%token SLASH_INVERTIDA

%token SALTO_LINEA

%token COMA

%type <listaValores> argument_list
%type <expresion> expresion
%type <identificador> identificador
%type <enunciado> statement

%%

program:
    statement_list
    ;

statement_list:
    statement { printf("> "); }
    | statement_list statement { printf("> "); }
    ;

argument_list:
    expresion {
            $$ = crear_lista_expresiones();
            push_lista_expresiones(&$$, $1);
        }
    | argument_list COMA expresion { push_lista_expresiones(&$$, $3); }
    ;

identifier_list:
    IDENTIFICADOR
    | identifier_list IDENTIFICADOR
    ;

function:
    SLASH_INVERTIDA argument_list FLECHA CORCHETE_IZQ statement_list CORCHETE_DER {}
    ;

expresion:
    IDENTIFICADOR { $$ = crear_exp_valor(crear_identificador($1)); }
    | ENTERO { $$ = crear_exp_valor(crear_entero($1)); }
    | OPERADOR_UNARIO expresion { $$ = crear_exp_op_unaria($1, $2); }
    | expresion OPERADOR_BINARIO expresion { $$ = crear_exp_op_binaria($1, $2, $3); }
    | IDENTIFICADOR PARENTESIS_IZQ argument_list PARENTESIS_DER { $$ = crear_exp_llamada($1, $3); }
    | PARENTESIS_IZQ expresion PARENTESIS_DER { $$ = $2; }
    | ERROR { $$ = crear_exp_valor(crear_error($1)); }
    ;

statement:
    expresion SALTO_LINEA {
            $$ = crear_enunciado_exp($1);
            imprimir_expresion($1);
        }
    | IDENTIFICADOR OPERADOR_ASIGNACION expresion {
            $$ = crear_enunciado_asignacion($1, $3);
        }
    ;

%%

void yyerror(const char* s) {
    printf("Error: %s\n", s);
}

int yywrap() {
    return 1;
}
