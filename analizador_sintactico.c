/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 1

/* Pull parsers.  */
#define YYPULL 0




/* First part of user prologue.  */
#line 1 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"

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


#line 99 "../analizador_sintactico.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "analizador_sintactico.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_ENTERO = 3,                     /* "número entero"  */
  YYSYMBOL_DECIMAL = 4,                    /* "número decimal"  */
  YYSYMBOL_IDENTIFICADOR = 5,              /* "identificador"  */
  YYSYMBOL_STRING = 6,                     /* "string"  */
  YYSYMBOL_ERROR = 7,                      /* ERROR  */
  YYSYMBOL_SUMA = 8,                       /* "+"  */
  YYSYMBOL_RESTA = 9,                      /* "-"  */
  YYSYMBOL_MULT = 10,                      /* "*"  */
  YYSYMBOL_DIV = 11,                       /* "/"  */
  YYSYMBOL_MOD = 12,                       /* "%"  */
  YYSYMBOL_IGUAL = 13,                     /* "="  */
  YYSYMBOL_EQ = 14,                        /* "=="  */
  YYSYMBOL_NEQ = 15,                       /* "!="  */
  YYSYMBOL_GE = 16,                        /* ">"  */
  YYSYMBOL_GEQ = 17,                       /* ">="  */
  YYSYMBOL_LE = 18,                        /* "<"  */
  YYSYMBOL_LEQ = 19,                       /* "<="  */
  YYSYMBOL_AND = 20,                       /* "&&"  */
  YYSYMBOL_OR = 21,                        /* "||"  */
  YYSYMBOL_NOT = 22,                       /* "!"  */
  YYSYMBOL_CONST = 23,                     /* "const"  */
  YYSYMBOL_EXPORT = 24,                    /* "export"  */
  YYSYMBOL_IMPORT = 25,                    /* "import"  */
  YYSYMBOL_FOREIGN = 26,                   /* "foreign"  */
  YYSYMBOL_AS = 27,                        /* "as"  */
  YYSYMBOL_BREAK = 28,                     /* "break"  */
  YYSYMBOL_RETURN = 29,                    /* "return"  */
  YYSYMBOL_IF = 30,                        /* "if"  */
  YYSYMBOL_THEN = 31,                      /* "then"  */
  YYSYMBOL_ELSE = 32,                      /* "else"  */
  YYSYMBOL_WHILE = 33,                     /* "while"  */
  YYSYMBOL_DO = 34,                        /* "do"  */
  YYSYMBOL_PARENTESIS_IZQ = 35,            /* "("  */
  YYSYMBOL_PARENTESIS_DER = 36,            /* ")"  */
  YYSYMBOL_CORCHETE_IZQ = 37,              /* "["  */
  YYSYMBOL_CORCHETE_DER = 38,              /* "]"  */
  YYSYMBOL_LLAVE_IZQ = 39,                 /* "{"  */
  YYSYMBOL_LLAVE_DER = 40,                 /* "}"  */
  YYSYMBOL_PUNTO = 41,                     /* "."  */
  YYSYMBOL_FLECHA = 42,                    /* "=>"  */
  YYSYMBOL_SLASH_INVERTIDA = 43,           /* "\\"  */
  YYSYMBOL_NUEVA_LINEA = 44,               /* "\n"  */
  YYSYMBOL_PUNTO_Y_COMA = 45,              /* ";"  */
  YYSYMBOL_COMA = 46,                      /* ","  */
  YYSYMBOL_YYACCEPT = 47,                  /* $accept  */
  YYSYMBOL_program = 48,                   /* program  */
  YYSYMBOL_nuevas_lineas = 49,             /* nuevas_lineas  */
  YYSYMBOL_argument_list_many = 50,        /* argument_list_many  */
  YYSYMBOL_argument_list = 51,             /* argument_list  */
  YYSYMBOL_identifier_list_many = 52,      /* identifier_list_many  */
  YYSYMBOL_identifier_list = 53,           /* identifier_list  */
  YYSYMBOL_expression_list_many = 54,      /* expression_list_many  */
  YYSYMBOL_expression_list = 55,           /* expression_list  */
  YYSYMBOL_acceso = 56,                    /* acceso  */
  YYSYMBOL_nombre_asignable = 57,          /* nombre_asignable  */
  YYSYMBOL_expresion = 58,                 /* expresion  */
  YYSYMBOL_statement = 59                  /* statement  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   482

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  47
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  13
/* YYNRULES -- Number of rules.  */
#define YYNRULES  75
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  140

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   301


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,   146,   146,   147,   148,   151,   151,   154,   155,   157,
     158,   161,   162,   164,   165,   168,   169,   172,   173,   177,
     178,   182,   183,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   240,   241,   242
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "\"número entero\"",
  "\"número decimal\"", "\"identificador\"", "\"string\"", "ERROR",
  "\"+\"", "\"-\"", "\"*\"", "\"/\"", "\"%\"", "\"=\"", "\"==\"", "\"!=\"",
  "\">\"", "\">=\"", "\"<\"", "\"<=\"", "\"&&\"", "\"||\"", "\"!\"",
  "\"const\"", "\"export\"", "\"import\"", "\"foreign\"", "\"as\"",
  "\"break\"", "\"return\"", "\"if\"", "\"then\"", "\"else\"", "\"while\"",
  "\"do\"", "\"(\"", "\")\"", "\"[\"", "\"]\"", "\"{\"", "\"}\"", "\".\"",
  "\"=>\"", "\"\\\\\"", "\"\\n\"", "\";\"", "\",\"", "$accept", "program",
  "nuevas_lineas", "argument_list_many", "argument_list",
  "identifier_list_many", "identifier_list", "expression_list_many",
  "expression_list", "acceso", "nombre_asignable", "expresion",
  "statement", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-40)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -40,   122,   -39,   -40,   -40,   -40,   -40,   -40,   -40,   -40,
     286,   286,     1,     1,    -5,   286,   286,   286,   286,   163,
     -40,     2,     6,    76,   -40,   -40,   -39,   447,   435,     9,
      65,   -40,     3,   435,   435,   351,   379,   -28,   204,   -25,
     -24,   -23,   -13,   -11,     4,     5,     8,    12,    15,    19,
     245,   322,   -39,   286,    -9,   -40,    -4,    17,   286,   286,
      40,   -40,   -40,   286,   286,   286,   286,   286,   286,   286,
     286,   286,   286,   286,   286,   286,   286,   -40,   -40,   -39,
     286,   286,    27,   286,   286,   -40,   -40,   -40,   -40,   -40,
     -40,   -40,   -40,   -40,   -40,   -40,   -40,   -40,   -40,   -40,
      76,   -40,   -40,    55,   286,   435,    18,   -40,   447,   447,
     200,   200,   200,   123,   123,   123,   123,   123,   123,   241,
      99,    31,    25,   435,   435,   435,    84,   407,   435,   -39,
     -39,   -40,   435,   -40,   286,   -40,   -40,   286,   435,   435
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       5,     0,     2,     1,     5,    23,    24,    21,    25,    72,
       0,     0,     0,     0,     0,    68,    70,     0,     0,     0,
       5,    14,    57,     0,     5,     6,     4,    41,    40,     0,
       0,    66,     0,    69,    71,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    15,    17,     0,    11,    13,     0,     0,     0,
       0,    22,    75,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    10,    74,    73,     3,
       0,     0,     0,     0,     0,    30,    31,    27,    28,    29,
      32,    33,    34,    35,    36,    37,    38,    39,    26,    56,
       5,     5,    61,     0,     0,    58,     0,    19,    45,    46,
      42,    43,    44,    47,    48,    49,    50,    51,    52,    53,
      54,     9,     0,     7,    59,    60,     0,    62,    64,    18,
      16,    12,    65,    20,     0,    55,    67,     0,     8,    63
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -40,   -40,     0,   -40,   -40,   -40,   -40,   -40,   -40,   -40,
     -10,    -1,    28
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     1,     2,   121,   122,    56,    57,    53,    54,    61,
      22,    27,    24
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      23,    31,    29,    30,    26,    25,     7,    55,    85,    82,
      28,    87,    88,    89,    33,    34,    35,    36,    51,    58,
      52,    32,    80,    90,    79,    91,    63,    64,    65,    66,
      67,   102,    68,    69,    70,    71,    72,    73,    74,    75,
      92,    93,   103,    59,    94,   107,    59,    60,    95,    28,
      60,    96,   100,    76,   126,    97,   133,   105,   106,   104,
     131,   135,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   123,    62,   134,    81,   124,
     125,   101,   127,   128,    63,    64,    65,    66,    67,   136,
      68,    69,    70,    71,    72,    73,    74,    75,     0,     0,
     129,   130,    59,   132,     0,     0,    60,     0,     0,     0,
       0,    76,     0,    68,    69,    70,    71,    72,    73,    74,
      77,    78,     3,     4,     0,     5,     6,     7,     8,     9,
       0,    10,     0,   138,    76,     0,   139,    -1,    -1,    -1,
      -1,    -1,    -1,     0,    11,    12,    13,    14,     0,     0,
      15,    16,    17,     0,     0,    18,     0,    19,    76,     0,
       0,    20,     0,     0,     0,    21,     5,     6,     7,     8,
       9,    37,    38,    39,    40,    41,     0,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    12,    13,    14,     0,
       0,    15,    16,    17,     0,     0,    18,     0,    19,     0,
       0,     0,    20,     0,     0,     0,    21,     5,     6,     7,
       8,     9,     0,    10,    68,    69,    70,    71,    72,    73,
      74,    75,     0,     0,     0,     0,    11,    12,    13,    14,
       0,     0,    15,    16,    17,    76,     0,    18,     0,    19,
      86,     0,     0,    20,     0,     0,     0,    21,     5,     6,
       7,     8,     9,     0,    10,    68,    69,    70,    71,    72,
      73,     0,     0,     0,     0,     0,     0,    11,    12,    13,
      14,     0,     0,    15,    16,    17,    76,     0,    18,     0,
      19,    98,     0,     0,    20,     0,     0,     0,    21,     5,
       6,     7,     8,     9,     0,    10,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    11,    12,
      13,    14,     0,     0,    15,    16,    17,     0,     0,    18,
       0,    19,     0,     0,     0,    20,     0,     0,     0,    21,
      63,    64,    65,    66,    67,     0,    68,    69,    70,    71,
      72,    73,    74,    75,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    76,    99,    63,
      64,    65,    66,    67,     0,    68,    69,    70,    71,    72,
      73,    74,    75,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    83,     0,     0,     0,    76,    63,    64,    65,
      66,    67,     0,    68,    69,    70,    71,    72,    73,    74,
      75,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    84,    76,    63,    64,    65,    66,    67,
       0,    68,    69,    70,    71,    72,    73,    74,    75,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   137,
       0,     0,    76,    63,    64,    65,    66,    67,     0,    68,
      69,    70,    71,    72,    73,    74,    75,    65,    66,    67,
       0,    68,    69,    70,    71,    72,    73,    74,    75,     0,
      76,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    76
};

static const yytype_int16 yycheck[] =
{
       1,     6,    12,    13,     4,    44,     5,     5,    36,     6,
      11,    36,    36,    36,    15,    16,    17,    18,    19,    13,
      20,    26,    13,    36,    24,    36,     8,     9,    10,    11,
      12,    40,    14,    15,    16,    17,    18,    19,    20,    21,
      36,    36,    46,    37,    36,     5,    37,    41,    36,    50,
      41,    36,    53,    35,    27,    36,    38,    58,    59,    42,
       5,    36,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,     0,    46,    13,    80,
      81,    53,    83,    84,     8,     9,    10,    11,    12,     5,
      14,    15,    16,    17,    18,    19,    20,    21,    -1,    -1,
     100,   101,    37,   104,    -1,    -1,    41,    -1,    -1,    -1,
      -1,    35,    -1,    14,    15,    16,    17,    18,    19,    20,
      44,    45,     0,     1,    -1,     3,     4,     5,     6,     7,
      -1,     9,    -1,   134,    35,    -1,   137,    14,    15,    16,
      17,    18,    19,    -1,    22,    23,    24,    25,    -1,    -1,
      28,    29,    30,    -1,    -1,    33,    -1,    35,    35,    -1,
      -1,    39,    -1,    -1,    -1,    43,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    -1,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    -1,
      -1,    28,    29,    30,    -1,    -1,    33,    -1,    35,    -1,
      -1,    -1,    39,    -1,    -1,    -1,    43,     3,     4,     5,
       6,     7,    -1,     9,    14,    15,    16,    17,    18,    19,
      20,    21,    -1,    -1,    -1,    -1,    22,    23,    24,    25,
      -1,    -1,    28,    29,    30,    35,    -1,    33,    -1,    35,
      36,    -1,    -1,    39,    -1,    -1,    -1,    43,     3,     4,
       5,     6,     7,    -1,     9,    14,    15,    16,    17,    18,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    22,    23,    24,
      25,    -1,    -1,    28,    29,    30,    35,    -1,    33,    -1,
      35,    36,    -1,    -1,    39,    -1,    -1,    -1,    43,     3,
       4,     5,     6,     7,    -1,     9,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    22,    23,
      24,    25,    -1,    -1,    28,    29,    30,    -1,    -1,    33,
      -1,    35,    -1,    -1,    -1,    39,    -1,    -1,    -1,    43,
       8,     9,    10,    11,    12,    -1,    14,    15,    16,    17,
      18,    19,    20,    21,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,    36,     8,
       9,    10,    11,    12,    -1,    14,    15,    16,    17,    18,
      19,    20,    21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    31,    -1,    -1,    -1,    35,     8,     9,    10,
      11,    12,    -1,    14,    15,    16,    17,    18,    19,    20,
      21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    34,    35,     8,     9,    10,    11,    12,
      -1,    14,    15,    16,    17,    18,    19,    20,    21,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,
      -1,    -1,    35,     8,     9,    10,    11,    12,    -1,    14,
      15,    16,    17,    18,    19,    20,    21,    10,    11,    12,
      -1,    14,    15,    16,    17,    18,    19,    20,    21,    -1,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    35
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    48,    49,     0,     1,     3,     4,     5,     6,     7,
       9,    22,    23,    24,    25,    28,    29,    30,    33,    35,
      39,    43,    57,    58,    59,    44,    49,    58,    58,    57,
      57,     6,    26,    58,    58,    58,    58,     8,     9,    10,
      11,    12,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    58,    49,    54,    55,     5,    52,    53,    13,    37,
      41,    56,     0,     8,     9,    10,    11,    12,    14,    15,
      16,    17,    18,    19,    20,    21,    35,    44,    45,    49,
      13,    13,     6,    31,    34,    36,    36,    36,    36,    36,
      36,    36,    36,    36,    36,    36,    36,    36,    36,    36,
      58,    59,    40,    46,    42,    58,    58,     5,    58,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    50,    51,    58,    58,    58,    27,    58,    58,    49,
      49,     5,    58,    38,    46,    36,     5,    32,    58,    58
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    47,    48,    48,    48,    49,    49,    50,    50,    51,
      51,    52,    52,    53,    53,    54,    54,    55,    55,    56,
      56,    57,    57,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    58,    59,    59,    59
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     3,     3,     0,     2,     1,     3,     1,
       0,     1,     3,     1,     0,     1,     3,     1,     3,     2,
       3,     1,     2,     1,     1,     1,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     4,     3,     1,     3,     4,
       4,     3,     4,     6,     4,     4,     2,     5,     1,     2,
       1,     2,     1,     2,     2,     2
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (&yylloc, exp, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, exp); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, Expresion *exp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (exp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, Expresion *exp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, exp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, Expresion *exp)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), exp);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, exp); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif
/* Parser data structure.  */
struct yypstate
  {
    /* Number of syntax errors so far.  */
    int yynerrs;

    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;
    /* Whether this instance has not started parsing yet.
     * If 2, it corresponds to a finished parsing.  */
    int yynew;
  };


/* Context of a parse error.  */
typedef struct
{
  yypstate* yyps;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypstate_expected_tokens (yypstate *yyps,
                          yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyps->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}


/* Similar to the previous function.  */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  return yypstate_expected_tokens (yyctx->yyps, yyarg, yyargn);
}


#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, Expresion *exp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (exp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yykind)
    {
    case YYSYMBOL_IDENTIFICADOR: /* "identificador"  */
#line 131 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
            { borrar_identificador(&((*yyvaluep).identificador)); }
#line 1422 "../analizador_sintactico.c"
        break;

    case YYSYMBOL_STRING: /* "string"  */
#line 132 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
            { borrar_string(&((*yyvaluep).string)); }
#line 1428 "../analizador_sintactico.c"
        break;

    case YYSYMBOL_argument_list_many: /* argument_list_many  */
#line 136 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
            { borrar_lista_expresiones(&((*yyvaluep).listaExpresiones)); }
#line 1434 "../analizador_sintactico.c"
        break;

    case YYSYMBOL_argument_list: /* argument_list  */
#line 135 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
            { borrar_lista_expresiones(&((*yyvaluep).listaExpresiones)); }
#line 1440 "../analizador_sintactico.c"
        break;

    case YYSYMBOL_identifier_list_many: /* identifier_list_many  */
#line 140 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
            { borrar_lista_identificadores(&((*yyvaluep).listaIdentificadores)); }
#line 1446 "../analizador_sintactico.c"
        break;

    case YYSYMBOL_identifier_list: /* identifier_list  */
#line 139 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
            { borrar_lista_identificadores(&((*yyvaluep).listaIdentificadores)); }
#line 1452 "../analizador_sintactico.c"
        break;

    case YYSYMBOL_expression_list_many: /* expression_list_many  */
#line 137 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
            { borrar_lista_expresiones(&((*yyvaluep).listaExpresiones)); }
#line 1458 "../analizador_sintactico.c"
        break;

    case YYSYMBOL_expression_list: /* expression_list  */
#line 138 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
            { borrar_lista_expresiones(&((*yyvaluep).listaExpresiones)); }
#line 1464 "../analizador_sintactico.c"
        break;

    case YYSYMBOL_acceso: /* acceso  */
#line 134 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
            { borrar_acceso(&((*yyvaluep).acceso)); }
#line 1470 "../analizador_sintactico.c"
        break;

    case YYSYMBOL_nombre_asignable: /* nombre_asignable  */
#line 133 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
            { borrar_nombre_asignable(&((*yyvaluep).nombreAsignable)); }
#line 1476 "../analizador_sintactico.c"
        break;

    case YYSYMBOL_expresion: /* expresion  */
#line 141 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
            { borrar_expresion(&((*yyvaluep).expresion)); }
#line 1482 "../analizador_sintactico.c"
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}





#define yynerrs yyps->yynerrs
#define yystate yyps->yystate
#define yyerrstatus yyps->yyerrstatus
#define yyssa yyps->yyssa
#define yyss yyps->yyss
#define yyssp yyps->yyssp
#define yyvsa yyps->yyvsa
#define yyvs yyps->yyvs
#define yyvsp yyps->yyvsp
#define yylsa yyps->yylsa
#define yyls yyps->yyls
#define yylsp yyps->yylsp
#define yystacksize yyps->yystacksize

/* Initialize the parser data structure.  */
static void
yypstate_clear (yypstate *yyps)
{
  yynerrs = 0;
  yystate = 0;
  yyerrstatus = 0;

  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

  /* Initialize the state stack, in case yypcontext_expected_tokens is
     called before the first call to yyparse. */
  *yyssp = 0;
  yyps->yynew = 1;
}

/* Initialize the parser data structure.  */
yypstate *
yypstate_new (void)
{
  yypstate *yyps;
  yyps = YY_CAST (yypstate *, YYMALLOC (sizeof *yyps));
  if (!yyps)
    return YY_NULLPTR;
  yystacksize = YYINITDEPTH;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yypstate_clear (yyps);
  return yyps;
}

void
yypstate_delete (yypstate *yyps)
{
  if (yyps)
    {
#ifndef yyoverflow
      /* If the stack was reallocated but the parse did not complete, then the
         stack still needs to be freed.  */
      if (yyss != yyssa)
        YYSTACK_FREE (yyss);
#endif
      YYFREE (yyps);
    }
}



/*---------------.
| yypush_parse.  |
`---------------*/

int
yypush_parse (yypstate *yyps,
              int yypushed_char, YYSTYPE const *yypushed_val, YYLTYPE *yypushed_loc, Expresion *exp)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  switch (yyps->yynew)
    {
    case 0:
      yyn = yypact[yystate];
      goto yyread_pushed_token;

    case 2:
      yypstate_clear (yyps);
      break;

    default:
      break;
    }

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = *yypushed_loc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      if (!yyps->yynew)
        {
          YYDPRINTF ((stderr, "Return for a new token:\n"));
          yyresult = YYPUSH_MORE;
          goto yypushreturn;
        }
      yyps->yynew = 0;
yyread_pushed_token:
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yypushed_char;
      if (yypushed_val)
        yylval = *yypushed_val;
      if (yypushed_loc)
        yylloc = *yypushed_loc;
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 3: /* program: program statement nuevas_lineas  */
#line 147 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                                      { *exp = (yyvsp[-1].expresion); }
#line 1851 "../analizador_sintactico.c"
    break;

  case 4: /* program: program error nuevas_lineas  */
#line 148 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                                  { }
#line 1857 "../analizador_sintactico.c"
    break;

  case 7: /* argument_list_many: expresion  */
#line 154 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
              { (yyval.listaExpresiones) = crear_lista_expresiones1((yyvsp[0].expresion)); }
#line 1863 "../analizador_sintactico.c"
    break;

  case 8: /* argument_list_many: argument_list_many "," expresion  */
#line 155 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                                       { push_lista_expresiones(&(yyvsp[-2].listaExpresiones), (yyvsp[0].expresion)); (yyval.listaExpresiones) = (yyvsp[-2].listaExpresiones); }
#line 1869 "../analizador_sintactico.c"
    break;

  case 10: /* argument_list: %empty  */
#line 158 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
             { (yyval.listaExpresiones) = crear_lista_expresiones(&(yyloc)); }
#line 1875 "../analizador_sintactico.c"
    break;

  case 11: /* identifier_list_many: "identificador"  */
#line 161 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                  { (yyval.listaIdentificadores) = crear_lista_identificadores1((yyvsp[0].identificador)); }
#line 1881 "../analizador_sintactico.c"
    break;

  case 12: /* identifier_list_many: identifier_list_many "," "identificador"  */
#line 162 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                                             { push_lista_identificadores(&(yyvsp[-2].listaIdentificadores), (yyvsp[0].identificador)); (yyval.listaIdentificadores) = (yyvsp[-2].listaIdentificadores); }
#line 1887 "../analizador_sintactico.c"
    break;

  case 14: /* identifier_list: %empty  */
#line 165 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
             { (yyval.listaIdentificadores) = crear_lista_identificadores(); }
#line 1893 "../analizador_sintactico.c"
    break;

  case 15: /* expression_list_many: nuevas_lineas  */
#line 168 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                    { (yyval.listaExpresiones) = crear_lista_expresiones(&(yyloc)); }
#line 1899 "../analizador_sintactico.c"
    break;

  case 16: /* expression_list_many: expression_list_many statement nuevas_lineas  */
#line 169 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                                                   { push_lista_expresiones(&(yyvsp[-2].listaExpresiones), (yyvsp[-1].expresion)); (yyval.listaExpresiones) = (yyvsp[-2].listaExpresiones); }
#line 1905 "../analizador_sintactico.c"
    break;

  case 17: /* expression_list: expression_list_many  */
#line 172 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                           { (yyval.listaExpresiones) = (yyvsp[0].listaExpresiones); }
#line 1911 "../analizador_sintactico.c"
    break;

  case 18: /* expression_list: expression_list_many expresion nuevas_lineas  */
#line 173 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                                                   { push_lista_expresiones(&(yyvsp[-2].listaExpresiones), (yyvsp[-1].expresion)); (yyval.listaExpresiones) = (yyvsp[-2].listaExpresiones); }
#line 1917 "../analizador_sintactico.c"
    break;

  case 19: /* acceso: "." "identificador"  */
#line 177 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                      { (yyval.acceso) = crear_acceso_miembro((yyvsp[0].identificador)); }
#line 1923 "../analizador_sintactico.c"
    break;

  case 20: /* acceso: "[" expresion "]"  */
#line 178 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                        { (yyval.acceso) = crear_acceso_indexado((struct Expresion*) &(yyvsp[-1].expresion)); }
#line 1929 "../analizador_sintactico.c"
    break;

  case 21: /* nombre_asignable: "identificador"  */
#line 182 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                  { (yyval.nombreAsignable) = crear_nombre_asignable((yyvsp[0].identificador)); }
#line 1935 "../analizador_sintactico.c"
    break;

  case 22: /* nombre_asignable: nombre_asignable acceso  */
#line 183 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                              { push_acceso_nombre_asignable(&(yyvsp[-1].nombreAsignable), (yyvsp[0].acceso)); (yyval.nombreAsignable) = (yyvsp[-1].nombreAsignable); }
#line 1941 "../analizador_sintactico.c"
    break;

  case 23: /* expresion: "número entero"  */
#line 187 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
             { (yyval.expresion) = crear_exp_valor(crear_entero((yyvsp[0].valorEntero), &(yylsp[0]))); }
#line 1947 "../analizador_sintactico.c"
    break;

  case 24: /* expresion: "número decimal"  */
#line 188 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
              { (yyval.expresion) = crear_exp_valor(crear_decimal((yyvsp[0].valorDecimal), &(yylsp[0]))); }
#line 1953 "../analizador_sintactico.c"
    break;

  case 25: /* expresion: "string"  */
#line 189 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
             { (yyval.expresion) = crear_exp_valor(crear_valor_string((yyvsp[0].string), &(yylsp[0]))); }
#line 1959 "../analizador_sintactico.c"
    break;

  case 26: /* expresion: "(" "!" ")"  */
#line 190 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                  { (yyval.expresion) = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_NOT, &(yyloc))); }
#line 1965 "../analizador_sintactico.c"
    break;

  case 27: /* expresion: "(" "*" ")"  */
#line 191 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                  { (yyval.expresion) = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_MULT, &(yyloc))); }
#line 1971 "../analizador_sintactico.c"
    break;

  case 28: /* expresion: "(" "/" ")"  */
#line 192 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                  { (yyval.expresion) = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_DIV, &(yyloc))); }
#line 1977 "../analizador_sintactico.c"
    break;

  case 29: /* expresion: "(" "%" ")"  */
#line 193 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                  { (yyval.expresion) = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_MOD, &(yyloc))); }
#line 1983 "../analizador_sintactico.c"
    break;

  case 30: /* expresion: "(" "+" ")"  */
#line 194 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                  { (yyval.expresion) = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_SUMA, &(yyloc))); }
#line 1989 "../analizador_sintactico.c"
    break;

  case 31: /* expresion: "(" "-" ")"  */
#line 195 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                  { (yyval.expresion) = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_RESTA, &(yyloc))); }
#line 1995 "../analizador_sintactico.c"
    break;

  case 32: /* expresion: "(" "==" ")"  */
#line 196 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                   { (yyval.expresion) = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_EQ, &(yyloc))); }
#line 2001 "../analizador_sintactico.c"
    break;

  case 33: /* expresion: "(" "!=" ")"  */
#line 197 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                   { (yyval.expresion) = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_NEQ, &(yyloc))); }
#line 2007 "../analizador_sintactico.c"
    break;

  case 34: /* expresion: "(" ">" ")"  */
#line 198 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                  { (yyval.expresion) = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_GE, &(yyloc))); }
#line 2013 "../analizador_sintactico.c"
    break;

  case 35: /* expresion: "(" ">=" ")"  */
#line 199 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                   { (yyval.expresion) = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_GEQ, &(yyloc))); }
#line 2019 "../analizador_sintactico.c"
    break;

  case 36: /* expresion: "(" "<" ")"  */
#line 200 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                  { (yyval.expresion) = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_LE, &(yyloc))); }
#line 2025 "../analizador_sintactico.c"
    break;

  case 37: /* expresion: "(" "<=" ")"  */
#line 201 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                   { (yyval.expresion) = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_LEQ, &(yyloc))); }
#line 2031 "../analizador_sintactico.c"
    break;

  case 38: /* expresion: "(" "&&" ")"  */
#line 202 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                   { (yyval.expresion) = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_AND, &(yyloc))); }
#line 2037 "../analizador_sintactico.c"
    break;

  case 39: /* expresion: "(" "||" ")"  */
#line 203 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                   { (yyval.expresion) = crear_exp_valor(crear_funcion_intrinseca(INTRINSECA_OR, &(yyloc))); }
#line 2043 "../analizador_sintactico.c"
    break;

  case 40: /* expresion: "!" expresion  */
#line 204 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                    { (yyval.expresion) = crear_exp_op_unaria(INTRINSECA_NOT, &(yylsp[0]), (yyvsp[0].expresion), &(yyloc)); }
#line 2049 "../analizador_sintactico.c"
    break;

  case 41: /* expresion: "-" expresion  */
#line 205 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                    { (yyval.expresion) = crear_exp_op_unaria(INTRINSECA_NEGAR, &(yylsp[0]), (yyvsp[0].expresion), &(yyloc)); }
#line 2055 "../analizador_sintactico.c"
    break;

  case 42: /* expresion: expresion "*" expresion  */
#line 206 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                              { (yyval.expresion) = crear_exp_op_binaria(INTRINSECA_MULT, &(yylsp[-1]), (yyvsp[-2].expresion), (yyvsp[0].expresion), &(yyloc)); }
#line 2061 "../analizador_sintactico.c"
    break;

  case 43: /* expresion: expresion "/" expresion  */
#line 207 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                              { (yyval.expresion) = crear_exp_op_binaria(INTRINSECA_DIV, &(yylsp[-1]), (yyvsp[-2].expresion), (yyvsp[0].expresion), &(yyloc)); }
#line 2067 "../analizador_sintactico.c"
    break;

  case 44: /* expresion: expresion "%" expresion  */
#line 208 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                              { (yyval.expresion) = crear_exp_op_binaria(INTRINSECA_MOD, &(yylsp[-1]), (yyvsp[-2].expresion), (yyvsp[0].expresion), &(yyloc)); }
#line 2073 "../analizador_sintactico.c"
    break;

  case 45: /* expresion: expresion "+" expresion  */
#line 209 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                              { (yyval.expresion) = crear_exp_op_binaria(INTRINSECA_SUMA, &(yylsp[-1]), (yyvsp[-2].expresion), (yyvsp[0].expresion), &(yyloc)); }
#line 2079 "../analizador_sintactico.c"
    break;

  case 46: /* expresion: expresion "-" expresion  */
#line 210 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                              { (yyval.expresion) = crear_exp_op_binaria(INTRINSECA_RESTA, &(yylsp[-1]), (yyvsp[-2].expresion), (yyvsp[0].expresion), &(yyloc)); }
#line 2085 "../analizador_sintactico.c"
    break;

  case 47: /* expresion: expresion "==" expresion  */
#line 211 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                               { (yyval.expresion) = crear_exp_op_binaria(INTRINSECA_EQ, &(yylsp[-1]), (yyvsp[-2].expresion), (yyvsp[0].expresion), &(yyloc)); }
#line 2091 "../analizador_sintactico.c"
    break;

  case 48: /* expresion: expresion "!=" expresion  */
#line 212 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                               { (yyval.expresion) = crear_exp_op_binaria(INTRINSECA_NEQ, &(yylsp[-1]), (yyvsp[-2].expresion), (yyvsp[0].expresion), &(yyloc)); }
#line 2097 "../analizador_sintactico.c"
    break;

  case 49: /* expresion: expresion ">" expresion  */
#line 213 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                              { (yyval.expresion) = crear_exp_op_binaria(INTRINSECA_GE, &(yylsp[-1]), (yyvsp[-2].expresion), (yyvsp[0].expresion), &(yyloc)); }
#line 2103 "../analizador_sintactico.c"
    break;

  case 50: /* expresion: expresion ">=" expresion  */
#line 214 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                               { (yyval.expresion) = crear_exp_op_binaria(INTRINSECA_GEQ, &(yylsp[-1]), (yyvsp[-2].expresion), (yyvsp[0].expresion), &(yyloc)); }
#line 2109 "../analizador_sintactico.c"
    break;

  case 51: /* expresion: expresion "<" expresion  */
#line 215 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                              { (yyval.expresion) = crear_exp_op_binaria(INTRINSECA_LE, &(yylsp[-1]), (yyvsp[-2].expresion), (yyvsp[0].expresion), &(yyloc)); }
#line 2115 "../analizador_sintactico.c"
    break;

  case 52: /* expresion: expresion "<=" expresion  */
#line 216 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                               { (yyval.expresion) = crear_exp_op_binaria(INTRINSECA_LEQ, &(yylsp[-1]), (yyvsp[-2].expresion), (yyvsp[0].expresion), &(yyloc)); }
#line 2121 "../analizador_sintactico.c"
    break;

  case 53: /* expresion: expresion "&&" expresion  */
#line 217 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                               { (yyval.expresion) = crear_exp_op_binaria(INTRINSECA_AND, &(yylsp[-1]), (yyvsp[-2].expresion), (yyvsp[0].expresion), &(yyloc)); }
#line 2127 "../analizador_sintactico.c"
    break;

  case 54: /* expresion: expresion "||" expresion  */
#line 218 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                               { (yyval.expresion) = crear_exp_op_binaria(INTRINSECA_OR, &(yylsp[-1]), (yyvsp[-2].expresion), (yyvsp[0].expresion), &(yyloc)); }
#line 2133 "../analizador_sintactico.c"
    break;

  case 55: /* expresion: expresion "(" argument_list ")"  */
#line 219 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                                      { (yyval.expresion) = crear_exp_llamada((yyvsp[-3].expresion), (yyvsp[-1].listaExpresiones), &(yyloc)); }
#line 2139 "../analizador_sintactico.c"
    break;

  case 56: /* expresion: "(" expresion ")"  */
#line 220 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                        { (yyval.expresion) = (yyvsp[-1].expresion); }
#line 2145 "../analizador_sintactico.c"
    break;

  case 57: /* expresion: nombre_asignable  */
#line 221 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                       { (yyval.expresion) = crear_exp_nombre((yyvsp[0].nombreAsignable)); }
#line 2151 "../analizador_sintactico.c"
    break;

  case 58: /* expresion: nombre_asignable "=" expresion  */
#line 222 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                                     { (yyval.expresion) = crear_exp_asignacion((yyvsp[-2].nombreAsignable), (yyvsp[0].expresion), ASIGNACION_NORMAL, &(yyloc)); }
#line 2157 "../analizador_sintactico.c"
    break;

  case 59: /* expresion: "const" nombre_asignable "=" expresion  */
#line 223 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                                             { (yyval.expresion) = crear_exp_asignacion((yyvsp[-2].nombreAsignable), (yyvsp[0].expresion), ASIGNACION_INMUTABLE, &(yyloc)); }
#line 2163 "../analizador_sintactico.c"
    break;

  case 60: /* expresion: "export" nombre_asignable "=" expresion  */
#line 224 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                                              { (yyval.expresion) = crear_exp_asignacion((yyvsp[-2].nombreAsignable), (yyvsp[0].expresion), ASIGNACION_EXPORT, &(yyloc)); }
#line 2169 "../analizador_sintactico.c"
    break;

  case 61: /* expresion: "{" expression_list "}"  */
#line 225 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                              { (yyval.expresion) = crear_exp_bloque((yyvsp[-1].listaExpresiones), &(yyloc)); }
#line 2175 "../analizador_sintactico.c"
    break;

  case 62: /* expresion: "if" expresion "then" expresion  */
#line 226 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                                      { (yyval.expresion) = crear_exp_condicional((yyvsp[-2].expresion), (yyvsp[0].expresion), NULL, &(yyloc)); }
#line 2181 "../analizador_sintactico.c"
    break;

  case 63: /* expresion: "if" expresion "then" expresion "else" expresion  */
#line 227 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                                                       { (yyval.expresion) = crear_exp_condicional((yyvsp[-4].expresion), (yyvsp[-2].expresion), &(yyvsp[0].expresion), &(yyloc)); }
#line 2187 "../analizador_sintactico.c"
    break;

  case 64: /* expresion: "while" expresion "do" expresion  */
#line 228 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                                       { (yyval.expresion) = crear_exp_bucle_while((yyvsp[-2].expresion), (yyvsp[0].expresion), &(yyloc)); }
#line 2193 "../analizador_sintactico.c"
    break;

  case 65: /* expresion: "\\" identifier_list "=>" expresion  */
#line 229 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                                          { (yyval.expresion) = crear_exp_def_funcion((yyvsp[-2].listaIdentificadores), (yyvsp[0].expresion), &(yyloc)); }
#line 2199 "../analizador_sintactico.c"
    break;

  case 66: /* expresion: "import" "string"  */
#line 230 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                      {(yyval.expresion) = crear_exp_importe((yyvsp[0].string), 0, &(yylsp[0])); }
#line 2205 "../analizador_sintactico.c"
    break;

  case 67: /* expresion: "import" "foreign" "string" "as" "identificador"  */
#line 231 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                                                   { (yyval.expresion) = crear_exp_importe_as((yyvsp[-2].string), 1, (yyvsp[0].identificador), &(yylsp[-2])); }
#line 2211 "../analizador_sintactico.c"
    break;

  case 68: /* expresion: "break"  */
#line 232 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
              { (yyval.expresion) = crear_exp_ctrl_flujo(CTR_FLUJO_BREAK, NULL, &(yylsp[0])); }
#line 2217 "../analizador_sintactico.c"
    break;

  case 69: /* expresion: "break" expresion  */
#line 233 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                        { (yyval.expresion) = crear_exp_ctrl_flujo(CTR_FLUJO_BREAK, &(yyvsp[0].expresion), &(yylsp[-1])); }
#line 2223 "../analizador_sintactico.c"
    break;

  case 70: /* expresion: "return"  */
#line 234 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
               { (yyval.expresion) = crear_exp_ctrl_flujo(CTR_FLUJO_RETURN, NULL, &(yylsp[0])); }
#line 2229 "../analizador_sintactico.c"
    break;

  case 71: /* expresion: "return" expresion  */
#line 235 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                         { (yyval.expresion) = crear_exp_ctrl_flujo(CTR_FLUJO_RETURN, &(yyvsp[0].expresion), &(yyloc)); }
#line 2235 "../analizador_sintactico.c"
    break;

  case 72: /* expresion: ERROR  */
#line 236 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
            { (yyval.expresion) = crear_exp_valor(crear_valor_error((yyvsp[0].error_lexico), &(yylsp[0]))); }
#line 2241 "../analizador_sintactico.c"
    break;

  case 73: /* statement: expresion ";"  */
#line 240 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                  { (yyval.expresion) = (yyvsp[-1].expresion); (yyval.expresion).es_sentencia = 1; }
#line 2247 "../analizador_sintactico.c"
    break;

  case 74: /* statement: expresion "\n"  */
#line 241 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                     { (yyval.expresion) = (yyvsp[-1].expresion);}
#line 2253 "../analizador_sintactico.c"
    break;

  case 75: /* statement: expresion $end  */
#line 242 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"
                      { (yyval.expresion) = (yyvsp[-1].expresion); }
#line 2259 "../analizador_sintactico.c"
    break;


#line 2263 "../analizador_sintactico.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyps, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (&yylloc, exp, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, exp);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, exp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, exp, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, exp);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, exp);
      YYPOPSTACK (1);
    }
  yyps->yynew = 2;
  goto yypushreturn;


/*-------------------------.
| yypushreturn -- return.  |
`-------------------------*/
yypushreturn:
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}
#undef yynerrs
#undef yystate
#undef yyerrstatus
#undef yyssa
#undef yyss
#undef yyssp
#undef yyvsa
#undef yyvs
#undef yyvsp
#undef yylsa
#undef yyls
#undef yylsp
#undef yystacksize
#line 244 "/home/andy/Documentos/USC/3º/CI/Practica3/analizador_sintactico.b"


void yyerror(Localizacion *loc, Expresion *exp, const char* s) {
    Error error = crear_error("%s", s);
    *exp = crear_exp_valor(crear_valor_error(error, loc));
}

int yywrap() {
    return 1;
}
