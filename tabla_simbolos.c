#include "tabla_simbolos.h"

#include "analizador_sintactico.h"
#include "string.h"

TablaSimbolos crear_tabla_simbolos() {
    TablaSimbolos t = {
            .capacidad = 1,
            .nivel = 0,
            .barreras = NULL,
            .longitud_barreras = 0,
            .capacidad_barreras = 0,
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
    // Comprobar si había una barrera establecida para este nivel.
    if (t->barreras && t->longitud_barreras > 0 && t->barreras[t->longitud_barreras-1] == t->nivel)
        t->longitud_barreras -= 1;

    // Borrar la tabla hash de este nivel.
    borrar_tabla_hash(&t->tablas[t->nivel]);
    --t->nivel;
}

void establecer_barrera_tabla_simbolos(TablaSimbolos *t) {
    // Comprobar si ya había una barrera establecida para este nivel.
    if (t->barreras && t->longitud_barreras > 0 && t->barreras[t->longitud_barreras-1] == t->nivel)
        return;

    ++t->longitud_barreras;
    if (t->longitud_barreras >= t->capacidad_barreras) {
        t->capacidad_barreras *= 2;
        t->barreras = realloc(t->barreras, t->capacidad*sizeof(int));
    }
    t->barreras[t->longitud_barreras-1] = t->nivel;
}

int nivel_ultima_barrera(TablaSimbolos t) {
    if (t.barreras && t.longitud_barreras > 0)
        return t.barreras[t.longitud_barreras-1];
    return 0;
}

void borrar_tabla_simbolos(TablaSimbolos *t) {
    for (int i = 0; i <= t->nivel; ++i) {
        borrar_tabla_hash(&t->tablas[i]);
    }
    free(t->tablas);
    if (t->barreras) free(t->barreras);
}

Valor* recuperar_valor_tabla(TablaSimbolos t, char *nombre, TipoAsignacion *tipo, int *nivel, int max_nivel) {
    for (int i = t.nivel; i >= max_nivel; --i) {
        EntradaTablaHash *resultado;
        if ((resultado = buscar_hash(t.tablas[i], nombre))) {
            if (tipo)
                *tipo = resultado->inmutable ? ASIGNACION_INMUTABLE : ASIGNACION_NORMAL;
            if (nivel)
                *nivel = i;
            return &resultado->valor;
        } else {
            continue;
        }
    }
    return 0;
}

Valor recuperar_clon_valor_tabla(TablaSimbolos t, Identificador nombre) {
    Valor *v;
    if ((v = recuperar_valor_tabla(t, identificador_a_str(&nombre), NULL, NULL, 0))) {
        return clonar_valor(*v);
    } else {
        Error error = crear_error_variable_no_definida(identificador_a_str(&nombre));
        return crear_valor_error(error, &nombre.loc);
    }
}

int asignar_valor_tabla(TablaSimbolos *t, String nombre, Valor valor, TipoAsignacion tipo) {
    int inmutable = tipo != ASIGNACION_NORMAL;

    // Tenemos que actuar de distintas maneras en base al tipo de asignación.
    switch (tipo) {
        case ASIGNACION_INMUTABLE: {
            // En una asignación inmutable siempre asignamos la variable en el nivel local.
            if (recuperar_valor_tabla(*t, string_a_str(&nombre), NULL, NULL, nivel_ultima_barrera(*t))) {
                // Si la variable ya estaba definida. Abortar
                borrar_valor(&valor);
                return 0;
            } else {
                // La variable no estaba definida. Insertarla como una variable local.
                insertar_hash(&t->tablas[t->nivel], nombre, valor, inmutable);
                return 1;
            }
        }
        case ASIGNACION_EXPORT: {
            // En una asignación de tipo export es igual a una inmutable, pero asignamos
            // si podemos en un nivel inferior de la tabla, haciendo que ésta pase a ser
            // una variable local del scope superior.
            int nivel = t->nivel > 0 ? t->nivel - 1: 0;
            // No comprobamos que la variable ya esté definida. Si reimportamos una
            // variable, no pasa nada.
            insertar_hash(&t->tablas[nivel], nombre, valor, inmutable);
            return 1;
        }
        case ASIGNACION_NORMAL: {
            int nivel_barrera = nivel_ultima_barrera(*t);

            Valor *actual;
            TipoAsignacion tipo_actual;
            int nivel;
            if ((actual = recuperar_valor_tabla(*t, string_a_str(&nombre), &tipo_actual, &nivel, nivel_barrera))) {
                // Si la variable ya estaba definida. Actualizar el valor que ya había si
                // la variable era mutable, y abortar si es inmutable.
                if (tipo_actual == ASIGNACION_NORMAL) {
                    borrar_valor(actual);
                    *actual = valor;
                    return 1;
                } else {
                    borrar_valor(&valor);
                    return 0;
                }
            } else {
                // La variable no estaba definida. Insertarla como una variable local.
                insertar_hash(&t->tablas[t->nivel], nombre, valor, inmutable);
                return 1;
            }
        }
    }
}

int asignar_clones_valores_tabla(TablaSimbolos *t, TablaHash otro) {
    int movidos = 0;

    int capacidad_real = otro.capacidad + otro.limite_busqueda;
    for (int i = 0; i < capacidad_real; ++i) {
        if (otro.buffer[i].distancia_posicion_ideal != -1) {
            EntradaTablaHash entrada = otro.buffer[i];
            String clave = clonar_string(entrada.clave);
            Valor v = clonar_valor(entrada.valor);
            if (asignar_valor_tabla(t, clave, v, entrada.inmutable ? ASIGNACION_INMUTABLE : ASIGNACION_NORMAL))
                ++movidos;
        }
    }
    return movidos;
}

void imprimir(EntradaTablaHash entrada) {
    printf("%s:= ", string_a_str(&entrada.clave));
    imprimir_valor(entrada.valor);
}

void imprimir_simbolos(TablaSimbolos t) {
    for (int i = 0; i <= t.nivel; ++i) {
        iterar_tabla_hash(t.tablas[i], imprimir);
    }
}