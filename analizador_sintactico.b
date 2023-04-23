%{
#include "string.h"
#include "ast/ast.h"

void yyerror(Localizacion *loc, Expresion *exp, const char* s);

# define YYLLOC_DEFAULT(Cur, Rhs, N)                      \
do                                                        \
  if (N)                                                  \
    {                                                     \
      (Cur).first_line   = YYRHSLOC(Rhs, 1).first_line;   \
      (Cur).first_column = YYRHSLOC(Rhs, 1).first_column; \
      (Cur).last_line    = YYRHSLOC(Rhs, N).last_line;    \
      (Cur).last_column  = YYRHSLOC(Rhs, N).last_column;  \
      (Cur).fuente       = YYRHSLOC(Rhs, N).fuente;       \
    }                                                     \
  else                                                    \
    {                                                     \
      (Cur).first_line   = (Cur).last_line   =            \
        YYRHSLOC(Rhs, 0).last_line;                       \
      (Cur).first_column = (Cur).last_column =            \
        YYRHSLOC(Rhs, 0).last_column;                     \
      (Cur).fuente = YYRHSLOC(Rhs, 0).fuente;             \
    }                                                     \
while (0)

%}

%parse-param {Expresion *exp}

%define parse.error verbose
%define api.pure full
%define api.push-pull push
%define api.location.type {Localizacion}

%locations

%code requires {
    #include "ast/ast.h"
}

%union {
    int valorEntero;
    double valorDecimal;
    Error error_lexico;
    Identificador identificador;
    Acceso acceso;
    NombreAsignable nombreAsignable;
    String string;
    ListaExpresiones listaExpresiones;
    ListaIdentificadores listaIdentificadores;
    Valor valor;
    Expresion expresion;
}

%token <valorEntero> ENTERO "número entero"
%token <valorDecimal> DECIMAL "número decimal"
%token <identificador> IDENTIFICADOR "identificador"
%token <string> STRING "string"
%token <error_lexico> ERROR

%token EQ "=="
%token NEQ "!="
%token GEQ ">="
%token LEQ "<="
%token AND "&&"
%token OR "||"
%token FLECHA "=>"

%token CONST "const"
%token EXPORT "export"
%token IMPORT "import"
%token FOREIGN "foreign"
%token AS "as"
%token BREAK "break"
%token RETURN "return"
%token IF "if"
%token THEN "then"
%token ELSE "else"
%token WHILE "while"
%token DO "do"

%type <listaExpresiones> argument_list
%type <listaExpresiones> argument_list_many
%type <listaExpresiones> expression_list
%type <listaExpresiones> expression_list_many
%type <listaIdentificadores> identifier_list
%type <listaIdentificadores> identifier_list_many
%type <expresion> expresion
%type <expresion> statement
%type <acceso> acceso
%type <nombreAsignable> nombre_asignable

%precedence "do"
%precedence "then"
%precedence "else"
%precedence "return"
%precedence "break"
%precedence '='
%precedence "=>"
%left "||"
%left "&&"
%nonassoc "==" "!=" '>' ">=" '<' "<="
%left '!'
%left '+' '-'
%left '*' '/' '%'
%precedence '('

%destructor { borrar_identificador(&$$); } IDENTIFICADOR
%destructor { borrar_string(&$$); } STRING
%destructor { borrar_nombre_asignable(&$$); } nombre_asignable
%destructor { borrar_acceso(&$$); } acceso
%destructor { borrar_lista_expresiones(&$$); } argument_list
%destructor { borrar_lista_expresiones(&$$); } argument_list_many
%destructor { borrar_lista_expresiones(&$$); } expression_list_many
%destructor { borrar_lista_expresiones(&$$); } expression_list
%destructor { borrar_lista_identificadores(&$$); } identifier_list
%destructor { borrar_lista_identificadores(&$$); } identifier_list_many
%destructor { borrar_expresion(&$$); } expresion

%%

program:
      %empty
    | program '\n'
    | program statement { *exp = $2; }
    ;

argument_list_many:
      expresion { $$ = crear_lista_expresiones1($1); }
    | argument_list_many ',' expresion { push_lista_expresiones(&$1, $3); $$ = $1; }
    ;
argument_list: argument_list_many
    | %empty { $$ = crear_lista_expresiones(&@$); }

identifier_list_many:
      IDENTIFICADOR { $$ = crear_lista_identificadores1($1); }
    | identifier_list_many ',' IDENTIFICADOR { push_lista_identificadores(&$1, $3); $$ = $1; }
    ;
identifier_list:  identifier_list_many
    | %empty { $$ = crear_lista_identificadores(); }

nuevas_lineas: %empty | nuevas_lineas '\n' ;

expression_list_many:
      %empty { $$ = crear_lista_expresiones(&@$); }
    | expression_list_many '\n'
    | expression_list_many statement { push_lista_expresiones(&$1, $2); $$ = $1; }
    ;
expression_list:
      expression_list_many { $$ = $1; }
    | expression_list_many expresion nuevas_lineas { push_lista_expresiones(&$1, $2); $$ = $1; }
    ;

acceso:
      '.' IDENTIFICADOR { $$ = crear_acceso_miembro($2); }
    | '[' expresion ']' { $$ = crear_acceso_indexado((struct Expresion*) &$2); }
    ;

nombre_asignable:
      IDENTIFICADOR { $$ = crear_nombre_asignable($1); }
    | nombre_asignable acceso { push_acceso_nombre_asignable(&$1, $2); $$ = $1; }
    ;

expresion:
      ENTERO { $$ = crear_exp_valor(crear_entero($1, &@1)); }
    | DECIMAL { $$ = crear_exp_valor(crear_decimal($1, &@1)); }
    | STRING { $$ = crear_exp_valor(crear_valor_string($1, &@1)); }
    | '(' '!' ')' { $$ = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_NOT, &@$)); }
    | '(' '*' ')' { $$ = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_MULT, &@$)); }
    | '(' '/' ')' { $$ = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_DIV, &@$)); }
    | '(' '%' ')' { $$ = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_MOD, &@$)); }
    | '(' '+' ')' { $$ = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_SUMA, &@$)); }
    | '(' '-' ')' { $$ = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_RESTA, &@$)); }
    | '(' "==" ')' { $$ = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_EQ, &@$)); }
    | '(' "!=" ')' { $$ = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_NEQ, &@$)); }
    | '(' '>' ')' { $$ = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_GE, &@$)); }
    | '(' ">=" ')' { $$ = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_GEQ, &@$)); }
    | '(' '<' ')' { $$ = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_LE, &@$)); }
    | '(' "<=" ')' { $$ = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_LEQ, &@$)); }
    | '(' "&&" ')' { $$ = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_AND, &@$)); }
    | '(' "||" ')' { $$ = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_OR, &@$)); }
    | '!' expresion { $$ = crear_exp_op_unaria(INTRINSECA_NOT, &@2, $2, &@$); }
    | '-' expresion { $$ = crear_exp_op_unaria(INTRINSECA_NEGAR, &@2, $2, &@$); }
    | expresion '*' expresion { $$ = crear_exp_op_binaria(INTRINSECA_MULT, &@2, $1, $3, &@$); }
    | expresion '/' expresion { $$ = crear_exp_op_binaria(INTRINSECA_DIV, &@2, $1, $3, &@$); }
    | expresion '%' expresion { $$ = crear_exp_op_binaria(INTRINSECA_MOD, &@2, $1, $3, &@$); }
    | expresion '+' expresion { $$ = crear_exp_op_binaria(INTRINSECA_SUMA, &@2, $1, $3, &@$); }
    | expresion '-' expresion { $$ = crear_exp_op_binaria(INTRINSECA_RESTA, &@2, $1, $3, &@$); }
    | expresion "==" expresion { $$ = crear_exp_op_binaria(INTRINSECA_EQ, &@2, $1, $3, &@$); }
    | expresion "!=" expresion { $$ = crear_exp_op_binaria(INTRINSECA_NEQ, &@2, $1, $3, &@$); }
    | expresion '>' expresion { $$ = crear_exp_op_binaria(INTRINSECA_GE, &@2, $1, $3, &@$); }
    | expresion ">=" expresion { $$ = crear_exp_op_binaria(INTRINSECA_GEQ, &@2, $1, $3, &@$); }
    | expresion '<' expresion { $$ = crear_exp_op_binaria(INTRINSECA_LE, &@2, $1, $3, &@$); }
    | expresion "<=" expresion { $$ = crear_exp_op_binaria(INTRINSECA_LEQ, &@2, $1, $3, &@$); }
    | expresion "&&" expresion { $$ = crear_exp_op_binaria(INTRINSECA_AND, &@2, $1, $3, &@$); }
    | expresion "||" expresion { $$ = crear_exp_op_binaria(INTRINSECA_OR, &@2, $1, $3, &@$); }
    | expresion '(' argument_list ')' { $$ = crear_exp_llamada($1, $3, &@$); }
    | '(' expresion ')' { $$ = $2; }
    | nombre_asignable { $$ = crear_exp_nombre($1); }
    | nombre_asignable '=' expresion { $$ = crear_exp_asignacion($1, $3, &@$); }
    | "const" IDENTIFICADOR '=' expresion { $$ = crear_exp_definicion($2, $4, 0, &@$); }
    | "export" IDENTIFICADOR '=' expresion { $$ = crear_exp_definicion($2, $4, 1, &@$); }
    | '{' expression_list '}' { $$ = crear_exp_bloque($2, &@$); }
    | "if" expresion "then" expresion { $$ = crear_exp_condicional($2, $4, NULL, &@$); }
    | "if" expresion "then" expresion "else" expresion { $$ = crear_exp_condicional($2, $4, &$6, &@$); }
    | "while" expresion "do" expresion { $$ = crear_exp_bucle_while($2, $4, &@$); }
    | '\\' identifier_list "=>" expresion { $$ = crear_exp_funcion($2, $4, &@$); }
    | "import" STRING {$$ = crear_exp_importe($2, 0, &@2); }
    | "import" "foreign" STRING "as" IDENTIFICADOR { $$ = crear_exp_importe_as($3, 1, $5, &@3); }
    | "break" { $$ = crear_exp_ctrl_flujo(CTR_FLUJO_BREAK, NULL, &@1); }
    | "break" expresion { $$ = crear_exp_ctrl_flujo(CTR_FLUJO_BREAK, &$2, &@1); }
    | "return" { $$ = crear_exp_ctrl_flujo(CTR_FLUJO_RETURN, NULL, &@1); }
    | "return" expresion { $$ = crear_exp_ctrl_flujo(CTR_FLUJO_RETURN, &$2, &@$); }
    | ERROR { $$ = crear_exp_valor(crear_valor_error($1, &@1)); }
    ;

statement:
      expresion ';'   { $$ = $1; $$.es_sentencia = 1; }
    | expresion '\n'  { $$ = $1;}
    | expresion YYEOF { $$ = $1; }

%%

void yyerror(Localizacion *loc, Expresion *exp, const char* s) {
    Error error = crear_error("%s", s);
    *exp = crear_exp_valor(crear_valor_error(error, loc));
}

int yywrap() {
    return 1;
}
