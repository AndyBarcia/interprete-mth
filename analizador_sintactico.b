%{
#include "analizador_lexico.h"
#include "string.h"
#include "evaluador.h"

void yyerror(TablaSimbolos tablaSimbolos, const char* s);

%}

%parse-param {TablaSimbolos tablaSimbolos}
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

%token <valorEntero> ENTERO "número entero"
%token <identificador> IDENTIFICADOR "identificador"
%token <error> ERROR

%token <tipoOperador> OPERADOR_ASIGNACION "operador de asignación"

%token CONST "const"

%token PARENTESIS_IZQ "("
%token PARENTESIS_DER ")"
%token CORCHETE_IZQ "["
%token CORCHETE_DER "]"
%token LLAVE_IZQ "{"
%token LLAVE_DER "}"

%token FLECHA "=>"
%token SLASH_INVERTIDA "\\"

%token NUEVA_LINEA "\n"
%token PUNTO_Y_COMA ";"

%token COMA ","

%type <identificador> nombre_asignable
%type <listaExpresiones> argument_list
%type <listaExpresiones> argument_list_many
%type <listaExpresiones> expression_block
%type <listaExpresiones> expression_block_inner
%type <listaIdentificadores> identifier_list
%type <listaIdentificadores> identifier_list_many
%type <expresion> expresion

%token <identificador> OPERADOR "operador"
%token <identificador> SUMA "+"
%token <identificador> MULT "*"

%precedence CONST
%precedence OPERADOR_ASIGNACION
%precedence "=>"
%left OPERADOR
%left "+"
%left "*"
%precedence "("

%%

program:
    statement_list
    ;

statement_list:
    expresion NUEVA_LINEA {
            imprimir_valor(evaluar_expresion(&tablaSimbolos, $1));
            //imprimir_expresion($1);
            printf("> ");
         }
    | statement_list expresion NUEVA_LINEA {
            imprimir_valor(evaluar_expresion(&tablaSimbolos, $2));
            //imprimir_expresion($2);
            printf("> ");
         }
    | statement_list expresion PUNTO_Y_COMA {
            evaluar_expresion(&tablaSimbolos, $2);
            //imprimir_expresion($2);
         }
    | error NUEVA_LINEA {
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
      argument_list_many
    | %empty { $$ = crear_lista_expresiones(); }

identifier_list_many:
    IDENTIFICADOR {
            $$ = crear_lista_identificadores();
            push_lista_identificadores(&$$, $1);
        }
    | identifier_list_many COMA IDENTIFICADOR { push_lista_identificadores(&$$, $3); }
    ;
identifier_list:
      identifier_list_many
    | %empty { $$ = crear_lista_identificadores(); }

sentence_terminator:
      sentence_terminator ";"
    | sentence_terminator "\n"
    | ";" | "\n"
    ;

nuevas_lineas: %empty | nuevas_lineas "\n"

expression_block:
     nuevas_lineas { $$ = crear_lista_expresiones(); }
    | expression_block expresion nuevas_lineas { push_lista_expresiones(&$$, $2); }
    ;

nombre_asignable: IDENTIFICADOR | OPERADOR ;

expresion:
    nombre_asignable { $$ = crear_exp_identificador($1); }
    | ENTERO { $$ = crear_exp_valor(crear_entero($1)); }
    | OPERADOR expresion { $$ = crear_exp_op_unaria($1, $2); }
    | expresion "*" expresion { $$ = crear_exp_op_binaria($2, $1, $3); }
    | expresion "+" expresion { $$ = crear_exp_op_binaria($2, $1, $3); }
    | expresion "(" argument_list ")" { $$ = crear_exp_llamada($1, $3); }
    | "(" expresion ")" { $$ = $2; }
    | nombre_asignable OPERADOR_ASIGNACION expresion { $$ = crear_exp_asignacion($1, $3, 0); }
    | "const" nombre_asignable OPERADOR_ASIGNACION expresion { $$ = crear_exp_asignacion($2, $4, 1); }
    | "{" expression_block "}" { $$ = crear_exp_bloque($2); }
    | "\\" identifier_list "=>" expresion { $$ = crear_exp_def_funcion($2, $4); }
    | expresion ";" { $$ = $1; $$.es_sentencia = 1; }
    | ERROR { $$ = crear_exp_valor(crear_error("%s", string_a_puntero(&$1))); }
    ;

%%

void yyerror(TablaSimbolos tablaSimbolos, const char* s) {
    printf("Error: %s\n", s);
}

int yywrap() {
    return 1;
}
