#include "tabla_simbolos.h"

#include "analizador_sintactico.h"
#include "string.h"

TablaSimbolos crear_tabla_simbolos() {
    TablaSimbolos t;
    t.capacidad = 1;
    t.nivel = 0;
    t.tablas = malloc(t.capacidad*sizeof(TablaHash));

    t.tablas[0] = crear_tabla_hash(16);
    return t;
}

void aumentar_nivel_tabla_simbolos(TablaSimbolos *t) {
    ++t->nivel;
    if (t->nivel >= t->capacidad) {
        t->capacidad *= 2;
        t->tablas = realloc(t->tablas, t->capacidad*sizeof(TablaHash));
    }
    t->tablas[t->nivel] = crear_tabla_hash(2);
}

void reducir_nivel_tabla_simbolos(TablaSimbolos *t) {
    borrar_tabla_hash(&t->tablas[t->nivel]);
    t->nivel--;
}

void borrar_tabla_simbolos(TablaSimbolos *t) {
    for (int i = 0; i <= t->nivel; ++i) {
        borrar_tabla_hash(&t->tablas[i]);
    }
}

Valor recuperar_valor_tabla(TablaSimbolos t, String identificador) {
    EntradaTablaHash resultado;
    for (int i = t.nivel; i >= 0; --i) {
        if (buscar_hash(t.tablas[i], string_a_puntero(&identificador), &resultado)) {
            return resultado.valor;
        } else {
            continue;
        }
    }
    return crear_error("\"%s\" es una variable no definida.", string_a_puntero(&identificador));
}

int asignar_valor_tabla(TablaSimbolos *t, String identificador, Valor valor, int inmutable) {
    // Buscar sólo en el último nivel si esta variable ya estaba definida como inmutable.
    EntradaTablaHash entrada;
    if (buscar_hash(t->tablas[t->nivel], string_a_puntero(&identificador), &entrada) && entrada.inmutable) {
        // Se estaba intentando reasignar una variable inmutable
        return 0;
    }

    insertar_hash(&t->tablas[t->nivel], identificador, valor, inmutable);
    return 1;
}

void imprimir(EntradaTablaHash entrada) {
    printf("%s:= ", string_a_puntero(&entrada.clave));
    imprimir_valor(entrada.valor);
}

void imprimir_simbolos(TablaSimbolos t) {
    for (int i = 0; i <= t.nivel; ++i) {
        iterar_tabla_hash(t.tablas[i], imprimir);
    }
}