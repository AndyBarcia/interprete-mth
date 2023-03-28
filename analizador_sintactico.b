%{
#include "analizador_lexico.h"
#include "string.h"
#include "evaluador.h"

void yyerror(TablaSimbolos tablaSimbolos, const char* s);

%}

%parse-param {TablaSimbolos tablaSimbolos}
%error-verbose
%define parse.error verbose

%code requires {
    #include "ast.h"
}

%union {
    int tipoOperador;
    int valorEntero;
    String error;
    String identificador;
    ListaExpresiones listaExpresiones;
    ListaIdentificadores listaIdentificadores;
    Valor valor;
    Expresion expresion;
}

%token <valorEntero> ENTERO
%token <identificador> IDENTIFICADOR
%token <error> ERROR

%token <identificador> OPERADOR
%token <tipoOperador> OPERADOR_ASIGNACION

%token CONST

%token PARENTESIS_IZQ
%token PARENTESIS_DER
%token CORCHETE_IZQ
%token CORCHETE_DER
%token LLAVE_IZQ
%token LLAVE_DER

%token FLECHA
%token SLASH_INVERTIDA

%token TERMINADOR_SENTENCIA

%token COMA

%type <identificador> nombre_asignable
%type <listaExpresiones> argument_list
%type <listaExpresiones> argument_list_many
%type <listaExpresiones> expression_block
%type <listaIdentificadores> identifier_list
%type <listaIdentificadores> identifier_list_many
%type <expresion> expresion

%%

program:
    statement_list
    ;

statement_list:
    expresion TERMINADOR_SENTENCIA {
            imprimir_valor(evaluar_expresion(&tablaSimbolos, $1));
            printf("> ");
        }
    | statement_list expresion TERMINADOR_SENTENCIA {
            imprimir_valor(evaluar_expresion(&tablaSimbolos, $2));
            printf("> ");
        }
    | error TERMINADOR_SENTENCIA {
            /* Saltarse la línea en caso de error */
            printf("> ");
        }
    ;

argument_list_many:
    expresion {
            $$ = crear_lista_expresiones();
            push_lista_expresiones(&$$, $1);
        }
    | argument_list_many COMA expresion { push_lista_expresiones(&$$, $3); }
    ;
argument_list:
    %empty { $$ = crear_lista_expresiones(); }
    | argument_list_many { $$ = $1; }

identifier_list_many:
    IDENTIFICADOR {
            $$ = crear_lista_identificadores();
            push_lista_identificadores(&$$, $1);
        }
    | identifier_list_many COMA IDENTIFICADOR { push_lista_identificadores(&$$, $3); }
    ;
identifier_list:
    %empty { $$ = crear_lista_identificadores(); }
    | identifier_list_many { $$ = $1; }

expression_block:
    expresion {
            $$ = crear_lista_expresiones();
            push_lista_expresiones(&$$, $1);
        }
    | expression_block TERMINADOR_SENTENCIA expresion { push_lista_expresiones(&$$, $3); }
    ;

nombre_asignable:
    IDENTIFICADOR { $$ = $1; }
    | OPERADOR { $$ = $1; }
    ;

expresion:
    nombre_asignable { $$ = crear_exp_identificador($1); }
    | ENTERO { $$ = crear_exp_valor(crear_entero($1)); }
    | OPERADOR expresion {
            ListaExpresiones args = crear_lista_expresiones();
            push_lista_expresiones(&args, $2);
            $$ = crear_exp_llamada($1, args);
        }
    | expresion OPERADOR expresion {
            ListaExpresiones args = crear_lista_expresiones();
            push_lista_expresiones(&args, $1);
            push_lista_expresiones(&args, $3);
            $$ = crear_exp_llamada($2, args);
        }
    | IDENTIFICADOR PARENTESIS_IZQ argument_list PARENTESIS_DER { $$ = crear_exp_llamada($1, $3); }
    | PARENTESIS_IZQ expresion PARENTESIS_DER { $$ = $2; }
    | nombre_asignable OPERADOR_ASIGNACION expresion { $$ = crear_exp_asignacion($1, $3, 0); }
    | CONST nombre_asignable OPERADOR_ASIGNACION expresion { $$ = crear_exp_asignacion($2, $4, 1); }
    | LLAVE_IZQ expression_block LLAVE_DER { $$ = crear_exp_bloque($2); }
    | SLASH_INVERTIDA identifier_list FLECHA expresion { $$ = crear_exp_valor(crear_funcion($2, $4)); }
    | ERROR { $$ = crear_exp_valor(crear_error("%s", string_a_puntero(&$1))); }
    ;

%%

void yyerror(TablaSimbolos tablaSimbolos, const char* s) {
    printf("Error: %s\n", s);
}

int yywrap() {
    return 1;
}
