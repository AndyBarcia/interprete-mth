#include "tabla_hash.h"

// Magia: https://stackoverflow.com/a/466278
unsigned long siguiente_potencia_de_dos(unsigned long v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

TablaHash crear_tabla_hash(int capacidad) {
    TablaHash t;
    t.longitud = 0;
    if (capacidad <= 0) capacidad = 1;
    t.capacidad = (int) siguiente_potencia_de_dos(capacidad);

    // Establecer el límite de búsqueda lineal como log2(capacidad).
    int x = t.capacidad;
    int limite = 1;
    while (x >>= 1) ++limite;
    t.limite_busqueda = limite;

    // Importante, reservamos un extra de slots de memoria;
    // los suficientes como para que no tengamos que comprobar
    // en la búsqueda e insercción si sobrepasamos el límite
    // del buffer.
    // https://probablydance.com/2017/02/26/i-wrote-the-fastest-hashtable/
    int capacidad_real = t.capacidad + t.limite_busqueda;
    t.buffer = malloc(sizeof(EntradaTablaHash) * capacidad_real);

    for (int i = 0; i < capacidad_real; i++) {
        t.buffer[i] = (EntradaTablaHash) {
                .valor = crear_valor_indefinido(),
                .clave = crear_string_vacio(),
                .distancia_posicion_ideal = -1
        };
    }

    return t;
}

void borrar_tabla_hash(TablaHash *t) {
    int capacidad_real = t->capacidad + t->limite_busqueda;
    for (int i = 0; i < capacidad_real; i++) {
        if (t->buffer[i].distancia_posicion_ideal != -1) {
            borrar_string(&t->buffer[i].clave);
            borrar_valor(&t->buffer[i].valor);
        }
    }
    free(t->buffer);
    t->buffer = NULL;
    t->longitud = 0;
    t->capacidad = 0;
}

unsigned int funcion_hash(char *cad) {
    // Hash FNV-1
    unsigned long hash = 14695981039346656037u;
    while (*cad) {
        hash = hash ^ (*cad);
        hash *= 1099511628211;
        ++cad;
    }

    return (unsigned int) hash;
}

EntradaTablaHash* buscar_hash(TablaHash t, char *identificador) {
    unsigned int hash = funcion_hash(identificador);
    int posicion_ideal = hash & (t.capacidad - 1);
    EntradaTablaHash *entrada_actual = &t.buffer[posicion_ideal];

    for (int distancia = 0; distancia < t.limite_busqueda; ++distancia) {
        int distancia_entrada_actual = entrada_actual->distancia_posicion_ideal;
        // Si este slot está a la misma distancia de su posición ideal que
        // nosotros, y tiene el mismo hash, seguramente es la entrada que buscamos.
        if (distancia_entrada_actual == distancia) {
            if (entrada_actual->hash == hash && strcmp(string_a_puntero(&entrada_actual->clave), identificador) == 0) {
                return entrada_actual;
            }
        } else if (distancia > distancia_entrada_actual) {
            // Si no, seguir buscando sólo mientras la distancia de la
            // entrada actual sea mayor que nuestra distancia de la
            // posición ideal.
            return 0;
        }

        // Nota: no tenemos que comprobar si "entrada_actual" sobrepasa
        // el final del buffer. Nunca puede pasar por diseño.
        // Reservamos "limite_busqueda" slots adicionales a la capacidad de la tabla.
        ++entrada_actual;
    }
    return NULL;
}

int es_miembro_hash(TablaHash t, char *clavebuscar) {
    return buscar_hash(t, clavebuscar) != NULL;
}

void crecer_tabla_hash(TablaHash *t, int nueva_capacidad);

void _insertar_hash_precalculado(TablaHash *t, String lexema, Valor valor, int inmutable, unsigned int hash) {
    int posicion_ideal = hash & (t->capacidad - 1);
    EntradaTablaHash *entrada_actual = &t->buffer[posicion_ideal];

    EntradaTablaHash entrada_a_insertar = {
            .valor = valor,
            .clave = lexema,
            .inmutable = inmutable,
            .distancia_posicion_ideal = 0,
            .hash = hash
    };

    while (1) {
        if (entrada_a_insertar.distancia_posicion_ideal > entrada_actual->distancia_posicion_ideal) {
            // Este slot o podría ser un hueco vacío, o una entrada que está a una menor
            // distancia de su posición ideal que nosotros. Comprobarlo.
            if (entrada_actual->distancia_posicion_ideal == -1) {
                *entrada_actual = entrada_a_insertar;
                ++t->longitud;
                return;
            } else {
                // Si no, encontramos una entrada que estaba más cerca
                // de su posición ideal que nosotros. Intercambiarla.
                EntradaTablaHash antiguo = *entrada_actual;
                *entrada_actual = entrada_a_insertar;
                entrada_a_insertar = antiguo;
            }
        } else if (entrada_a_insertar.distancia_posicion_ideal == entrada_actual->distancia_posicion_ideal &&
                   entrada_a_insertar.hash == entrada_actual->hash) {
            // Este slot podría tener la misma clave que nosotros, pues está a la misma distancia
            // de la posición ideal y tiene el mismo hash. Comparar las claves para asegurarnos.
            if (strcmp(string_a_puntero(&entrada_actual->clave), string_a_puntero(&entrada_a_insertar.clave)) == 0) {
                // Liberar la memoria que ya estaba en la entrada, porque la vamos
                // a sobreescribir con la que nos pasó como argumento.
                borrar_string(&entrada_actual->clave);
                borrar_valor(&entrada_actual->valor);
                *entrada_actual = entrada_a_insertar;
                return;
            }
        }
        ++entrada_a_insertar.distancia_posicion_ideal;
        ++entrada_actual;

        // Si hemos superalo el límite de búsqueda de un hueco, hacer crecer la tabla.
        if (entrada_a_insertar.distancia_posicion_ideal > t->limite_busqueda) {
            crecer_tabla_hash(t, t->capacidad * 2);
            // Resetear los datos de la entrada que estamos insertando
            entrada_a_insertar.distancia_posicion_ideal = 0;
            posicion_ideal = entrada_a_insertar.hash & (t->capacidad - 1);
            entrada_actual = &t->buffer[posicion_ideal];
        }
    }
}

void crecer_tabla_hash(TablaHash *t, int nueva_capacidad) {
    // Crear una nueva tabla hash con el doble de capacidad.
    TablaHash nueva_tabla = crear_tabla_hash(nueva_capacidad);
    // Mover todos los elementos de la tabla antigua a la nueva.
    int capacidad_real = t->capacidad + t->limite_busqueda;
    for (int i = 0; i < capacidad_real; i++) {
        if (t->buffer[i].distancia_posicion_ideal != -1) {
            // Aprovechar que ya tenemos precalculados los
            // hashes de todas las entradas y no tenemos que
            // volver a calcularlos.
            _insertar_hash_precalculado(&nueva_tabla, t->buffer[i].clave, t->buffer[i].valor, t->buffer[i].inmutable, t->buffer[i].hash);
        }
    }
    nueva_tabla.longitud = t->longitud;

    // Eliminar la tabla hash actual y sustituirla por la nueva.
    free(t->buffer);
    *t = nueva_tabla;
}

Valor insertar_hash(TablaHash *t, String identificador, Valor valor, int inmutable) {
    // Si el hashmap se va a llenar al 90%, aumentar su tamaño.
    // Esto  no debería de pasar de forma normal, porque ya se
    // incrementa el tamaño de la tabla si hay que buscar demasiados
    // elementos para encontrar un hueco.
    if ((100 * t->longitud) >= (90 * t->capacidad)) {
        crecer_tabla_hash(t, t->capacidad * 2);
    }

    // Calcular el hash del lexema e insertarlo en la tabla.
    unsigned int hash = funcion_hash(string_a_puntero(&identificador));
    _insertar_hash_precalculado(t, identificador, valor, inmutable, hash);
    return valor;
}

/* Funcion que elimina un elemento de la tabla */
int borrar_hash(TablaHash *t, char *claveborrar) {
    printf("Me da pereza implementar algo que nunca uso.");
    return 0;
}

void iterar_tabla_hash(TablaHash t, void (*visitador)(EntradaTablaHash entrada)) {
    int capacidad_real = t.capacidad + t.limite_busqueda;
    for (int i = 0; i < capacidad_real; ++i) {
        if (t.buffer[i].distancia_posicion_ideal != -1) {
            visitador(t.buffer[i]);
        }
    }
}

void imprimir_tabla_hash(TablaHash t) {
    int capacidad_real = t.capacidad + t.limite_busqueda;
    for (int i = 0; i < capacidad_real; ++i) {
        if (t.buffer[i].distancia_posicion_ideal == -1) {
            printf("[  \t\t]\n");
            continue;
        }
        printf("[ \"%s\" \t]", string_a_puntero(&t.buffer[i].clave));
        if (t.buffer[i].distancia_posicion_ideal == 0) {
            printf(" <-- ");
        } else {
            printf(" ^%d^ ", t.buffer[i].distancia_posicion_ideal);
        }
        imprimir_valor(t.buffer[i].valor);
    }
}
