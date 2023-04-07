#include "tabla_simbolos.h"

#include "analizador_sintactico.h"
#include "string.h"

TablaSimbolos crear_tabla_simbolos() {
    TablaSimbolos t = {
            .capacidad = 1,
            .nivel = 0,
    };
    t.tablas = malloc(t.capacidad*sizeof(TablaHash));
    t.tablas[t.nivel] = crear_tabla_hash(16);
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
    free(t->tablas);
}

Valor recuperar_valor_tabla(TablaSimbolos t, Identificador  identificador) {
    EntradaTablaHash resultado;
    for (int i = t.nivel; i >= 0; --i) {
        if (buscar_hash(t.tablas[i], string_a_puntero(&identificador.nombre), &resultado)) {
            return clonar_valor(resultado.valor);
        } else {
            continue;
        }
    }
    Error error = crear_error("\"%s\" es una variable no definida.", string_a_puntero(&identificador.nombre));
    return crear_valor_error(error, &identificador.loc);
}

int asignar_valor_tabla(TablaSimbolos *t, Identificador identificador, Valor valor, TipoAsignacion tipo) {
    // Buscar sólo en el último nivel si esta variable ya estaba definida como inmutable.
    EntradaTablaHash entrada;

    // El nivel en el que se asignará la variable.
    // De forma normal, se hace en el último nivel de la tabla; pero si
    // se está exportando, se hace en un nivel inferior.
    int nivel = t->nivel;
    if (nivel > 0 && tipo == ASIGNACION_EXPORT) --nivel;

    if (buscar_hash(t->tablas[nivel], string_a_puntero(&identificador.nombre), &entrada) && entrada.inmutable) {
        // Se estaba intentando reasignar una variable inmutable
        borrar_string(&identificador.nombre);
        borrar_valor(&valor);
        return 0;
    }

    int inmutable = tipo != ASIGNACION_NORMAL;
    insertar_hash(&t->tablas[nivel], identificador.nombre, valor, inmutable);
    return 1;
}

int asignar_clones_valores_tabla(TablaSimbolos *t, TablaHash otro) {
    int capacidad_real = otro.capacidad + otro.limite_busqueda;
    for (int i = 0; i < capacidad_real; ++i) {
        if (otro.buffer[i].distancia_posicion_ideal != -1) {
            EntradaTablaHash entrada = otro.buffer[i];
            String clave = clonar_string(entrada.clave);
            Identificador id = (Identificador) { .nombre = clave };
            Valor v = clonar_valor(entrada.valor);
            if (!asignar_valor_tabla(t, id, v, entrada.inmutable ? ASIGNACION_INMUTABLE : ASIGNACION_NORMAL))
                return 0;
        }
    }
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