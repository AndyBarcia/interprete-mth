%{
#include "analizador_lexico.h"
#include "string.h"
#include "ast.h"

void yyerror(void *loc, Expresion *exp, const char* s);

#define localization(x) ((Localizacion) {        \
    .first_line = x.first_line,      \
    .first_column = x.first_column,  \
    .last_line = x.last_line,        \
    .last_column = x.last_column,    \
 })

%}

%parse-param {Expresion *exp}

%define parse.error verbose
%define api.pure full
%define api.push-pull push

%locations

%code requires {
    #include "ast.h"
}

%union {
    int tipoOperador;
    int valorEntero;
    String error;
    String identificador;
    String string;
    ListaExpresiones listaExpresiones;
    ListaIdentificadores listaIdentificadores;
    Valor valor;
    Expresion expresion;
}

%token <valorEntero> ENTERO "número entero"
%token <identificador> IDENTIFICADOR "identificador"
%token <string> STRING "string"
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

%destructor { borrar_string(&$$); } IDENTIFICADOR
%destructor { borrar_string(&$$); } STRING
%destructor { borrar_string(&$$); } nombre_asignable
%destructor { borrar_lista_expresiones(&$$); } argument_list
%destructor { borrar_lista_expresiones(&$$); } argument_list_many
%destructor { borrar_lista_expresiones(&$$); } expression_block
%destructor { borrar_lista_identificadores(&$$); } identifier_list
%destructor { borrar_lista_identificadores(&$$); } identifier_list_many
%destructor { borrar_expresion(&$$); } expresion

%%

program:
    statement_list
    ;

statement_list:
    %empty {  }
    | expresion { *exp = $1; }
    | error { }
    | "\n" statement_list { }
    | statement_list "\n" expresion { *exp = $3; }
    | statement_list "\n" error { }
    ;

argument_list_many:
    expresion {
            $$ = crear_lista_expresiones();
            push_lista_expresiones(&$$, $1);
        }
    | argument_list_many COMA expresion { push_lista_expresiones(&$1, $3); $$ = $1; }
    ;
argument_list:
      argument_list_many
    | %empty { $$ = crear_lista_expresiones(); }

identifier_list_many:
    IDENTIFICADOR {
            $$ = crear_lista_identificadores();
            push_lista_identificadores(&$$, $1);
        }
    | identifier_list_many COMA IDENTIFICADOR { push_lista_identificadores(&$1, $3); $$ = $1; }
    ;
identifier_list:
      identifier_list_many
    | %empty { $$ = crear_lista_identificadores(); }

nuevas_lineas: %empty | nuevas_lineas "\n"

expression_block:
     nuevas_lineas { $$ = crear_lista_expresiones(); }
    | expression_block expresion nuevas_lineas { push_lista_expresiones(&$1, $2); $$ = $1; }
    ;

nombre_asignable: IDENTIFICADOR | OPERADOR ;

expresion:
    nombre_asignable { $$ = crear_exp_identificador($1, localization(@1)); }
    | ENTERO { $$ = crear_exp_valor(crear_entero($1), localization(@1)); }
    | STRING { $$ = crear_exp_valor(crear_valor_string($1), localization(@1)); }
    | OPERADOR expresion { $$ = crear_exp_op_unaria($1, $2, localization(@1)); }
    | expresion "*" expresion { $$ = crear_exp_op_binaria($2, $1, $3, localization(@1)); }
    | expresion "+" expresion { $$ = crear_exp_op_binaria($2, $1, $3, localization(@1)); }
    | expresion "(" argument_list ")" { $$ = crear_exp_llamada($1, $3, localization(@1)); }
    | "(" expresion ")" { $$ = $2; }
    | nombre_asignable OPERADOR_ASIGNACION expresion { $$ = crear_exp_asignacion($1, $3, 0, localization(@1)); }
    | "const" nombre_asignable OPERADOR_ASIGNACION expresion { $$ = crear_exp_asignacion($2, $4, 1, localization(@1)); }
    | "{" expression_block "}" { $$ = crear_exp_bloque($2, localization(@1)); }
    | "\\" identifier_list "=>" expresion { $$ = crear_exp_def_funcion($2, $4, localization(@1)); }
    | expresion ";" { $$ = $1; $$.es_sentencia = 1; }
    | ERROR { $$ = crear_exp_valor(crear_error("%s", string_a_puntero(&$1)), localization(@1)); }
    ;

%%

void yyerror(void *loc, Expresion *exp, const char* s) {
    YYLTYPE yyloc = *(YYLTYPE*) loc;
    *exp = crear_exp_valor(crear_error("%s", s), localization(yyloc));
}

int yywrap() {
    return 1;
}
