%{
#include <stdio.h>
#include <stdarg.h>
#include "string.h"
#include "analizador_lexico.h"
#include "analizador_sintactico.h"
#include "tabla_simbolos.h"
#include "error.h"

#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno; \
                       yylloc->first_column = yylloc->last_column; \
                       yylloc->last_column += yyleng;

#define OP_SUMAR 128
#define OP_RESTAR 129
#define OP_COMPARACION_IGUAL 130
#define OP_COMPARACION_MAYOR_IGUAL 131
#define OP_COMPARACION_MENOR_IGUAL 132

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
%option reentrant bison-bridge bison-locations yylineno
%%

const { return CONST; }
export { return EXPORT; }
import { return IMPORT; }
foreign { return FOREIGN; }

{ALFA_US}{ALFANUM_US}* {
         yylval->identificador = crear_string(yytext);
         return IDENTIFICADOR;
     }

{INICIO_ENTERO}{CUERPO_ENTERO} {
        yylval->valorEntero = atoi(yytext);
        return ENTERO;
    }
{INICIO_ENTERO}{CUERPO_ENTERO_MALFORMADO} {
        yylval->error_lexico = crear_error("Número \"%s\" malformado.", yytext);
        return ERROR;
    }

{STRING} {
        yytext[yyleng-1] = '\0';
        // TODO: escapar caracteres
        yylval->string = crear_string(yytext+1);
        return STRING;
    }
{STRING_MALFORMADO} {
        yylval->error_lexico = crear_error("String sin comillas de cierre.");
        return ERROR;
    }

[\-/.<>&|] {
        yylval->identificador = crear_string(yytext);
        return OPERADOR;
    }
\+  {
        yylval->identificador = crear_string(yytext);
        return SUMA;
    }
\*  {
        yylval->identificador = crear_string(yytext);
        return MULT;
    }
\=\=   {
        yylval->identificador = crear_string(yytext);
        return OPERADOR;
    }
>=  {
        yylval->identificador = crear_string(yytext);
        return OPERADOR;
    }
\<=  {
        yylval->identificador = crear_string(yytext);
        return OPERADOR;
    }
!   {
        yylval->identificador = crear_string(yytext);
        return OPERADOR;
    }
= {
        yylval->tipoOperador = '=';
        return OPERADOR_ASIGNACION;
    }
\+= {
        yylval->tipoOperador = OP_SUMAR;
        return OPERADOR_ASIGNACION;
    }
-= {
        yylval->tipoOperador = OP_RESTAR;
        return OPERADOR_ASIGNACION;
    }

\(   { return PARENTESIS_IZQ; }
\)   { return PARENTESIS_DER; }
\[   { return CORCHETE_IZQ; }
\]   { return CORCHETE_DER; }
\{   { return LLAVE_IZQ; }
\}   { return LLAVE_DER; }

,   { return COMA; }

\n  {
        yylloc->last_column = 1;
        return NUEVA_LINEA;
    }
;  { return PUNTO_Y_COMA; }

=>  { return FLECHA;}
\\  { return SLASH_INVERTIDA; }

\/\/[^\n]* { /* ignorar comentarios de una línea */ }

\/\* { BEGIN(comentario_bloque); }
<comentario_bloque>{
\*\/ { BEGIN(INITIAL); }
\n  { yylloc->last_column = 1; }
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
\n  { yylloc->last_column = 1; }
.   { /* ignorar comentarios anidados */ }
<<EOF>> {
        BEGIN(INITIAL);
        yylval->error_lexico = crear_error("Comentario anidado no terminado.");
        return ERROR;
    }
}

{ESPACIO_EN_BLANCO} { /* ignorar espacios */ }

<<EOF>> { return YYEOF; }

.   {
        yylval->error_lexico = crear_error("Caracter no reconocido.");
        return ERROR;
    }
%%

int crear_lexer_archivo(Lexer *lexer, char *archivo) {
    FILE* fichero;
    if ((fichero = fopen(archivo, "r")) == NULL) {
        return 0;
    }

    lexer->nombre_fichero = archivo;
    lexer->str_buffer = NULL;

    yylex_init(&lexer->scanner);
    yyset_in(fichero, lexer->scanner);
    return 1;
}

Lexer crear_lexer_str(char *str) {
     Lexer lexer;
     lexer.nombre_fichero = NULL;
     yylex_init(&lexer.scanner);

     lexer.str_buffer = yy_scan_string(str, lexer.scanner);

     return lexer;
}

void borrar_analizador_lexico(Lexer lexer) {
    if (lexer.str_buffer) {
        yy_delete_buffer(lexer.str_buffer, lexer.scanner);
    } else if (yyget_in(lexer.scanner) != stdin) {
        fclose(yyget_in(lexer.scanner));
    }
    yylex_destroy(lexer.scanner);
}

char* obtener_linea(Lexer lexer, int linea) {
    if (lexer.str_buffer) {
        // TODO hacer esto
        return NULL;
    } else {
        FILE* archivo = yyget_in(lexer.scanner);

        // Guardar posición actual en el archivo y movernos al inicio
        long int pos = ftell(archivo);
        fseek(archivo, 0, SEEK_SET);

        char* buffer = NULL;
        size_t bufsize = 0;
        int current_line = 0;

        while (current_line < linea && getline(&buffer, &bufsize, archivo) != -1) {
            current_line++;
        }

        if (current_line < linea) {
            free(buffer);
            return NULL;
        }

        // Volver a la posición original y terminar.
        fseek(archivo, pos, SEEK_SET);
        return buffer;
    }
}

char* obtener_nombre_fichero(Lexer lexer) {
    return lexer.nombre_fichero;
}

int siguiente_componente_lexico(Lexer lexer, void* token, Localizacion *loc) {
    return yylex((YYSTYPE*) token, loc, lexer.scanner);
}