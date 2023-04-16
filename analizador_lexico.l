%{
#include <stdio.h>
#include <stdarg.h>
#include "string.h"
#include "analizador_sintactico.h"
#include "tabla_simbolos.h"
#include "error.h"

#define YY_USER_ACTION yylloc->first_line = yylloc->last_line; \
                       yylloc->first_column = yylloc->last_column; \
                       yylloc->last_column += yyleng;

int nivel_comentario_anidado = 0;
%}

ALFA [a-zA-Z]
ALFA_US [a-zA-Z_]
DIGITO [0-9]

ALFANUM ({ALFA}|{DIGITO})
ALFANUM_US ({ALFA}|{DIGITO}|_)

IDENTIFICADOR ({ALFA_US}{ALFANUM_US}*)

ESPACIO_EN_BLANCO [ \t\n]

ENTERO {DIGITO}+

DIGITO_BINARIO [01]
ENTERO_BINARIO 0[bB]{DIGITO_BINARIO}+

DIGITO_HEXA [0-9a-fA-F]
ENTERO_HEXA 0[xX]{DIGITO_HEXA}+

DECIMAL {DIGITO}+\.{DIGITO}*
DECIMAL_PUNTO \.{DIGITO}+

EXPONENTE [eE][+-]?{DIGITO}+

NUMERO_MALFORMADO {DIGITO}({ALFANUM_US}|".")+

STRING \"(\\.|[^\n"\\])*\"
STRING_MALFORMADO \"(\\.|[^\n"\\])*

%x comentario_anidado
%x comentario_bloque
%option reentrant bison-bridge bison-locations
%%

const   { return CONST; }
export  { return EXPORT; }
import  { return IMPORT; }
foreign { return FOREIGN; }
as      { return AS; }
break   { return BREAK; }
return  { return RETURN; }
if      { return IF; }
then    { return THEN; }
else    { return ELSE; }

{IDENTIFICADOR} {
         yylval->identificador = crear_identificador(crear_string(yytext), clonar_loc(*yylloc));
         return IDENTIFICADOR;
     }

{ENTERO} {
        yylval->valorEntero = atoi(yytext);
        return ENTERO;
    }
{ENTERO_BINARIO} {
        yylval->valorEntero = strtoul(yytext+2, NULL, 2);
        return ENTERO;
    }
{ENTERO_HEXA} {
        yylval->valorEntero = strtoul(yytext+2, NULL, 16);
        return ENTERO;
    }

(({DECIMAL}|{DECIMAL_PUNTO}){EXPONENTE}?)|{ENTERO}{EXPONENTE} {
        yylval->valorDecimal = atof(yytext);
        return DECIMAL;
    }

{NUMERO_MALFORMADO} {
        yylval->error_lexico = crear_error("Número \"%s\" malformado.", yytext);
        return ERROR;
    }

{STRING} {
        yytext[yyleng-1] = '\0';
        yylval->string = crear_string_escapado(yytext+1);
        return STRING;
    }
{STRING_MALFORMADO} {
        yylval->error_lexico = crear_error("String sin comillas de cierre.");
        return ERROR;
    }

\+   { return SUMA; }
-    { return RESTA; }
\*   { return MULT; }
\/   { return DIV; }
%    { return MOD; }

\=\= { return EQ; }
!\=  { return NEQ; }
>    { return GE; }
>=   { return GEQ; }
\<   { return LE; }
\<=  { return LEQ; }
&&   { return AND; }
\|\| { return OR; }
!    { return NOT; }
\=   { return IGUAL; }

\(   { return PARENTESIS_IZQ; }
\)   { return PARENTESIS_DER; }
\[   { return CORCHETE_IZQ; }
\]   { return CORCHETE_DER; }
\{   { return LLAVE_IZQ; }
\}   { return LLAVE_DER; }
;    { return PUNTO_Y_COMA; }
\.   { return PUNTO; }
,    { return COMA; }
=>  { return FLECHA;}
\\  { return SLASH_INVERTIDA; }

\n  {
        yylloc->last_column = 1;
        ++yylloc->last_line;
        return NUEVA_LINEA;
    }

\/\/[^\n]* { /* ignorar comentarios de una línea */ }

\/\* { BEGIN(comentario_bloque); }
<comentario_bloque>{
\*\/ { BEGIN(INITIAL); }
\n  {
        ++yylloc->last_line;
        yylloc->last_column = 1;
    }
.   { /* ignorar comentarios multilinea */ }
<<EOF>> {
        BEGIN(INITIAL);
        yylval->error_lexico = crear_error("Comentario de bloque no terminado.");
        return ERROR;
    }
}

\/\+ {
        nivel_comentario_anidado=1;
        BEGIN(comentario_anidado);
    }
<comentario_anidado>{
\/\+ { ++nivel_comentario_anidado; }
\+\/  {
        --nivel_comentario_anidado;
        if (nivel_comentario_anidado == 0) BEGIN(INITIAL);
    }
\n  {
        ++yylloc->last_line;
        yylloc->last_column = 1;
    }
.   { /* ignorar comentarios anidados */ }
<<EOF>> {
        BEGIN(INITIAL);
        yylval->error_lexico = crear_error("Comentario anidado no terminado.");
        return ERROR;
    }
}

{ESPACIO_EN_BLANCO} { /* ignorar espacios */ }

<<EOF>> {
        yylloc->first_column = yylloc->last_column;
        yylloc->last_column++;
        return YYEOF;
    }

.   {
        yylval->error_lexico = crear_error("Caracter no reconocido.");
        return ERROR;
    }
%%

Lexer crear_lexer(CodigoFuente codigo_fuente) {
    Lexer lexer;
    yylex_init(&lexer.scanner);

    lexer.loc = (Localizacion) {
             .first_line = 1,
             .last_line = 1,
             .first_column = 1,
             .last_column = 1,
             .fuente = codigo_fuente
     };

     switch (codigo_fuente.tipo) {
         case SRC_ARCHIVO:
             yyset_in(codigo_fuente.src.archivo.fd, lexer.scanner);
             lexer.str_buffer = NULL;
             break;
         case SRC_STR:
             lexer.str_buffer = yy_scan_string(codigo_fuente.src.str, lexer.scanner);
             break;
     }
     return lexer;
}

void borrar_analizador_lexico(Lexer *lexer) {
    if (lexer->str_buffer) {
        yy_delete_buffer(lexer->str_buffer, lexer->scanner);
    }
    borrar_codigo_fuente(&lexer->loc.fuente);
    yylex_destroy(lexer->scanner);
}

Localizacion obtener_localizacion(Lexer lexer) {
    return lexer.loc;
}

int siguiente_componente_lexico(Lexer *lexer, void* token) {
    return yylex((YYSTYPE*) token, &lexer->loc, lexer->scanner);
}