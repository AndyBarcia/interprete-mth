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
#line 38 "analizador_sintactico.b"

    #include "ast/ast.h"

#line 53 "analizador_sintactico.h"

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
    DECIMAL = 259,                 /* "número decimal"  */
    IDENTIFICADOR = 260,           /* "identificador"  */
    STRING = 261,                  /* "string"  */
    ERROR = 262,                   /* ERROR  */
    SUMA = 263,                    /* "+"  */
    RESTA = 264,                   /* "-"  */
    MULT = 265,                    /* "*"  */
    DIV = 266,                     /* "/"  */
    MOD = 267,                     /* "%"  */
    IGUAL = 268,                   /* "="  */
    EQ = 269,                      /* "=="  */
    NEQ = 270,                     /* "!="  */
    GE = 271,                      /* ">"  */
    GEQ = 272,                     /* ">="  */
    LE = 273,                      /* "<"  */
    LEQ = 274,                     /* "<="  */
    AND = 275,                     /* "&&"  */
    OR = 276,                      /* "||"  */
    NOT = 277,                     /* "!"  */
    CONST = 278,                   /* "const"  */
    EXPORT = 279,                  /* "export"  */
    IMPORT = 280,                  /* "import"  */
    FOREIGN = 281,                 /* "foreign"  */
    AS = 282,                      /* "as"  */
    BREAK = 283,                   /* "break"  */
    RETURN = 284,                  /* "return"  */
    IF = 285,                      /* "if"  */
    THEN = 286,                    /* "then"  */
    ELSE = 287,                    /* "else"  */
    WHILE = 288,                   /* "while"  */
    DO = 289,                      /* "do"  */
    PARENTESIS_IZQ = 290,          /* "("  */
    PARENTESIS_DER = 291,          /* ")"  */
    CORCHETE_IZQ = 292,            /* "["  */
    CORCHETE_DER = 293,            /* "]"  */
    LLAVE_IZQ = 294,               /* "{"  */
    LLAVE_DER = 295,               /* "}"  */
    PUNTO = 296,                   /* "."  */
    FLECHA = 297,                  /* "=>"  */
    SLASH_INVERTIDA = 298,         /* "\\"  */
    NUEVA_LINEA = 299,             /* "\n"  */
    PUNTO_Y_COMA = 300,            /* ";"  */
    COMA = 301                     /* ","  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 42 "analizador_sintactico.b"

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

#line 130 "analizador_sintactico.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
typedef Localizacion YYLTYPE;




#ifndef YYPUSH_MORE_DEFINED
# define YYPUSH_MORE_DEFINED
enum { YYPUSH_MORE = 4 };
#endif

typedef struct yypstate yypstate;


int yypush_parse (yypstate *ps,
                  int pushed_char, YYSTYPE const *pushed_val, YYLTYPE *pushed_loc, Expresion *exp);

yypstate *yypstate_new (void);
void yypstate_delete (yypstate *ps);


#endif /* !YY_YY_ANALIZADOR_SINTACTICO_H_INCLUDED  */
