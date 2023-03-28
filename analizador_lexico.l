%{
#include <stdio.h>
#include <stdarg.h>
#include "string.h"
#include "analizador_lexico.h"
#include "analizador_sintactico.h"
#include "tabla_simbolos.h"

#define OP_SUMAR 128
#define OP_RESTAR 129
#define OP_COMPARACION_IGUAL 130
#define OP_COMPARACION_MAYOR_IGUAL 131
#define OP_COMPARACION_MENOR_IGUAL 132

TablaSimbolos tablaSimbolos;

void inicializar_analizador_lexico() {
    tablaSimbolos = crear_tabla_simbolos();
}

int yylex_destroy (void);

void borrar_analizador_lexico() {
    borrar_tabla_simbolos(&tablaSimbolos);
    fclose(yyin);
    yylex_destroy();
}

String crear_error_lexico(const char *formato, ...) {
    String error;
    // Cosas mágicas para pasar un número variable de argumentos.
    va_list args, args2;
    va_start(args, formato);
    va_copy(args2, args);
    // Calcular la longitud total que tendrá el string final.
    size_t longitud = vsnprintf(NULL, 0, formato, args);
    error = crear_string_n(longitud);
    // Escribir el resultado en el string creado.
    char *str = string_a_puntero(&error);
    vsnprintf(str, longitud + 1, formato, args2);
    va_end(args);
    va_end(args2);

    return error;
}

int nivel_comentario_anidado = 0;
%}

ALFA [a-zA-Z]
ALFA_US [a-zA-Z_]
DIGITO [0-9]
DIGITO_US [0-9_]

ALFANUM ({ALFA}|{DIGITO})
ALFANUM_US ({ALFA}|{DIGITO}|_)

ESPACIO_EN_BLANCO [ \t\n]

INICIO_ENTERO_BINARIO 0[bB]
DIGITO_BINARIO [01]
DIGITO_BINARIO_US [01_]
CUERPO_ENTERO_BINARIO ({DIGITO_BINARIO}{DIGITO_BINARIO_US}*)|(_{DIGITO_BINARIO_US}*{DIGITO_BINARIO}{DIGITO_BINARIO_US}*)

INICIO_ENTERO_HEXA 0[xX]
DIGITO_HEXA [0-9a-fA-F]
DIGITO_HEXA_US [0-9a-fA-F_]
CUERPO_ENTERO_HEXA ({DIGITO_HEXA}{DIGITO_HEXA_US}*)|(_{DIGITO_HEXA_US}*{DIGITO_HEXA}{DIGITO_HEXA_US}*)

INICIO_ENTERO {DIGITO}
CUERPO_ENTERO {DIGITO_US}*
CUERPO_ENTERO_MALFORMADO {ALFANUM_US}+

EXPONENTE [eE][+-]?{DIGITO_US}+
EXPONENTE_MALFORMADO [eE][+-]?{ALFANUM_US}*
DECIMAL {DIGITO}{DIGITO_US}*\.{DIGITO}?{DIGITO_US}*
DECIMAL_PUNTO \.{DIGITO}{DIGITO_US}*
DECIMAL_MALFORMADO {DIGITO}({ALFANUM_US}|".")+

STRING \"(\\.|[^\n"\\])*\"
STRING_MALFORMADO \"(\\.|[^\n"\\])*

OPERADORES [+\-*/=,.;{}()\[\]<>!&|]

%x comentario_anidado
%x comentario_bloque
%%
{ALFA_US}{ALFANUM_US}* {
         yylval.identificador = crear_string(yytext);
         return IDENTIFICADOR;
     }

{INICIO_ENTERO}{CUERPO_ENTERO} {
        yylval.valorEntero = atoi(yytext);
        return ENTERO;
    }
{INICIO_ENTERO}{CUERPO_ENTERO_MALFORMADO} {
        yylval.error = crear_error_lexico("Número \"%s\" malformado.", yytext);
        return ERROR;
    }

[+\-*/.<>&|] {
        yylval.tipoOperador = yytext[0];
        return OPERADOR_BINARIO;
    }
\=\=   {
        yylval.tipoOperador = OP_COMPARACION_IGUAL;
        return OPERADOR_BINARIO;
    }
>=  {
        yylval.tipoOperador = OP_COMPARACION_MAYOR_IGUAL;
        return OPERADOR_BINARIO;
    }
\<=  {
        yylval.tipoOperador = OP_COMPARACION_MENOR_IGUAL;
        return OPERADOR_BINARIO;
    }
!   {
        yylval.tipoOperador = yytext[0];
        return OPERADOR_UNARIO;
    }
= {
        yylval.tipoOperador = '=';
        return OPERADOR_ASIGNACION;
    }
\+= {
        yylval.tipoOperador = OP_SUMAR;
        return OPERADOR_ASIGNACION;
    }
-= {
        yylval.tipoOperador = OP_RESTAR;
        return OPERADOR_ASIGNACION;
    }

\(   { return PARENTESIS_IZQ; }
\)   { return PARENTESIS_DER; }
\[   { return CORCHETE_IZQ; }
\]   { return CORCHETE_DER; }
\{   { return LLAVE_IZQ; }
\}   { return LLAVE_DER; }

,   { return COMA; }

\n|;  { return TERMINADOR_SENTENCIA; }

=>  { return FLECHA;}
\\  { return SLASH_INVERTIDA; }

\/\/[^\n]* { /* ignorar comentarios de una línea */ }

\/\* { BEGIN(comentario_bloque); }
<comentario_bloque>{
\*\/ { BEGIN(INITIAL); }
.|\n { /* ignorar comentarios multilinea */ }
<<EOF>> {
        BEGIN(INITIAL);
        yylval.error = crear_error_lexico("Comentario de bloque no terminado.");
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
.|\n { /* ignorar comentarios anidados */ }
<<EOF>> {
        BEGIN(INITIAL);
        yylval.error = crear_error_lexico("Comentario anidado no terminado.");
        return ERROR;
    }
}

{ESPACIO_EN_BLANCO} { /* ignorar espacios */ }

.   {
        yylval.error = crear_error_lexico("Caracter no reconocido.");
        return ERROR;
    }
%%

void establecer_fichero_entrada(FILE *archivo) {
    yyin = archivo;
}