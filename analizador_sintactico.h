/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_ANALIZADOR_SINTACTICO_H_INCLUDED
# define YY_YY_ANALIZADOR_SINTACTICO_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 13 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"

    #include "ast.h"

#line 53 "../analizador_sintactico.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ENTERO = 258,                  /* "número entero"  */
    IDENTIFICADOR = 259,           /* "identificador"  */
    ERROR = 260,                   /* ERROR  */
    OPERADOR = 261,                /* "operador"  */
    OPERADOR_ASIGNACION = 262,     /* "operador de asignación"  */
    CONST = 263,                   /* "const"  */
    PARENTESIS_IZQ = 264,          /* "("  */
    PARENTESIS_DER = 265,          /* ")"  */
    CORCHETE_IZQ = 266,            /* "["  */
    CORCHETE_DER = 267,            /* "]"  */
    LLAVE_IZQ = 268,               /* "{"  */
    LLAVE_DER = 269,               /* "}"  */
    FLECHA = 270,                  /* "=>"  */
    SLASH_INVERTIDA = 271,         /* "\\"  */
    NUEVA_LINEA = 272,             /* "salto de línea"  */
    PUNTO_Y_COMA = 273,            /* "punto y coma"  */
    COMA = 274                     /* COMA  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 17 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"

    int tipoOperador;
    int valorEntero;
    String error;
    String identificador;
    ListaExpresiones listaExpresiones;
    ListaIdentificadores listaIdentificadores;
    Valor valor;
    Expresion expresion;

#line 100 "../analizador_sintactico.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (TablaSimbolos tablaSimbolos);


#endif /* !YY_YY_ANALIZADOR_SINTACTICO_H_INCLUDED  */
