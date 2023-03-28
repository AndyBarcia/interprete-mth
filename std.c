#include "std.h"
#include "ast.h"

void sumar(ListaValores args, Valor *retorno) {
    int a = 0;
    for (int i = 0; i < args.longitud; ++i) {
        if (((Valor*)args.valores)[i].tipoValor != TIPO_ENTERO) {
            *retorno = crear_error("No es de tipo entero.");
            return;
        }
        a += ((Valor*)args.valores)[i].entero;
    }
    *retorno = crear_entero(a);
}

typedef struct {
    FuncionNativa funcion;
    char* texto_ayuda;
} TextoAyuda;

void ayuda(ListaValores args, Valor *retorno);

TextoAyuda ayudas[] = {
        { sumar, "Suma un conjunto de enteros." },
        { ayuda, "Imprime la documentación de una función de la librería estándar." }
};

void ayuda(ListaValores args, Valor *retorno) {
    if (args.longitud > 1) {
        *retorno = crear_error("Sólo se acepta un argumento de entrada.");
        return;
    }
    if (args.longitud == 0) {
        printf("Imprime ayuda sobre la función que se pasa como argumento. Ejemplo: `ayuda(sumar)`.\n");
        return;
    }
    Valor arg = ((Valor*)args.valores)[0];
    if (arg.tipoValor != TIPO_FUNCION_NATIVA && arg.tipoValor != TIPO_FUNCION) {
        *retorno = crear_error("No es una función.");
        return;
    }
    if (arg.tipoValor != TIPO_FUNCION_NATIVA) {
        *retorno = crear_error("No es una función de la librería estándar.");
        return;
    }
    for (int i = 0; i < sizeof(ayudas)/sizeof(TextoAyuda); ++i) {
        if (((Valor*)args.valores)[0].funcion_nativa == ayudas[i].funcion) {
            printf("%s\n", ayudas[i].texto_ayuda);
            return;
        }
    }
    printf("No hay ayuda para la función.ºn");
}

void inicializar_libreria_estandar(TablaSimbolos *t) {
    asignar_valor_tabla(t, crear_string("sumar"), crear_funcion_nativa(sumar));
    asignar_valor_tabla(t, crear_string("+"), crear_funcion_nativa(sumar));
    asignar_valor_tabla(t, crear_string("ayuda"), crear_funcion_nativa(ayuda));
}