#include <stdio.h>

#include "analizador_lexico.h"
#include "analizador_sintactico.h"
#include "tabla_hash.h"
#include "string.h"
#include "std.h"
#include "evaluador.h"

int main(int argc, char *argv[]) {


    FILE* entrada;
    if (argc <= 1) {
        entrada = stdin;
    }else {
        if (argc > 2) printf("Demasiados argumentos; ignorando.\n");

        char *fichero = argv[1];
        printf("Abriendo \"%s\"\n", fichero);
        if ((entrada = fopen(fichero, "r")) == NULL) {
            printf("No se pudo abrir el archivo \"%s\"\n", fichero);
            exit(1);
        }
    }

    crear_buffer_strings();
    Lexer lexer = crear_analizador_lexico(entrada);

    TablaSimbolos simbolos = crear_tabla_simbolos();
    inicializar_libreria_estandar(&simbolos);

    if (entrada == stdin) printf("> ");

    int status;
    yypstate *ps = yypstate_new();
    do {
        YYSTYPE yylval_param;
        int c = siguiente_componente_lexico(lexer, &yylval_param);

        Expresion exp;
        exp.tipo = -1;
        status = yypush_parse(ps, c, &yylval_param, &exp);

        if (exp.tipo != -1) {
            Valor v = evaluar_expresion(&simbolos, exp);
            if (v.tipoValor == TIPO_ERROR || entrada == stdin) imprimir_valor(v);
            if (entrada == stdin) printf("> ");
        }
    } while (status == YYPUSH_MORE);
    yypstate_delete (ps);

    printf("Terminado!!!!!\n");

    borrar_analizador_lexico(lexer);
    borrar_buffer_strings();

    /*
yyscan_t scanner;
YY_BUFFER_STATE buf;
yylex_init( &scanner );
yytext[yyleng-1] = ’ ’;
buf = yy_scan_string( yytext + 5, scanner );
yylex( scanner );
yy_delete_buffer(buf,scanner);
yylex_destroy( scanner );
     */

    return 0;
}