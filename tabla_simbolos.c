#include "tabla_simbolos.h"

#include "definiciones.h"
#include "analizador_sintactico.h"
#include "string.h"

typedef struct {
    char *palabra;
    TipoLexema tipo;
} PalabraClave;

PalabraClave palabras_clave[] = {
};

TablaSimbolos crear_tabla_simbolos() {
    TablaSimbolos t;
    t.tablaHash = crear_tabla_hash(sizeof(palabras_clave) / sizeof(PalabraClave));
    for (int i = 0; i < sizeof(palabras_clave) / sizeof(PalabraClave); ++i) {
        insertar_hash(&t.tablaHash, (ComponenteLexico) {
                .tipo = palabras_clave[i].tipo,
                .lexema = crear_string(palabras_clave[i].palabra)
        });
    }
    return t;
}

void borrar_tabla_simbolos(TablaSimbolos *t) {
    borrar_tabla_hash(&t->tablaHash);
}

ComponenteLexico buscar_o_insertar_simbolo(TablaSimbolos *t, String simbolo) {
    ComponenteLexico entrada = (ComponenteLexico) {
            .tipo = IDENTIFICADOR,
            .lexema = simbolo
    };
    ComponenteLexico salida;
    if (buscar_o_insertar_hash(&t->tablaHash, entrada, &salida)) {
        // El elemento ya estaba en la tabla, devolver lo que estaba ahí
        // y liberar la memoria del símbolo, que no se utilizará.
        borrar_string(&simbolo);
        return salida;
    } else {
        // Insertamos el elemento.
        return entrada;
    };
}

void imprimir(EntradaTablaHash entrada) {
    printf("<%s: %d>\n", string_a_puntero(&entrada.clave), entrada.tipo);
}

void imprimir_simbolos(TablaSimbolos t) {
    iterar_tabla_hash(t.tablaHash, imprimir);
}