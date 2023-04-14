#include "identificador.h"
#include <stdlib.h>

Identificador crear_identificador(String nombre, Localizacion loc) {
    return (Identificador) {
            .nombre = nombre,
            .loc = loc
    };
}

void borrar_identificador(Identificador *identificador) {
    borrar_string(&identificador->nombre);
    borrar_loc(&identificador->loc);
}

Identificador clonar_identificador(Identificador id) {
    return (Identificador) {
            .nombre = clonar_string(id.nombre),
            .loc = clonar_loc(id.loc)
    };
}

char* identificador_a_str(Identificador *id) {
    return string_a_puntero(&id->nombre);
}

ListaIdentificadores crear_lista_identificadores() {
    return (ListaIdentificadores) {
            .longitud = 0,
            .capacidad = 0,
            .valores = NULL
    };
}

ListaIdentificadores crear_lista_identificadores1(Identificador identificador) {
    ListaIdentificadores lista = crear_lista_identificadores();
    push_lista_identificadores(&lista, identificador);
    return lista;
}

void push_lista_identificadores(ListaIdentificadores *lista, Identificador identificador) {
    if (lista->longitud >= lista->capacidad) {
        lista->valores = realloc(lista->valores, (lista->capacidad + 1) * sizeof(Identificador));
        ++lista->capacidad;
    }
    ((Identificador *) lista->valores)[lista->longitud++] = identificador;
}

ListaIdentificadores clonar_lista_identificadores(ListaIdentificadores lista) {
    ListaIdentificadores copia = (ListaIdentificadores) {
            .longitud = lista.longitud,
            .capacidad = lista.longitud,
            .valores = malloc(sizeof(Identificador) * lista.longitud)
    };

    for (int i = 0; i < lista.longitud; ++i)
        copia.valores[i] = clonar_identificador(lista.valores[i]);
    return copia;
}

void borrar_lista_identificadores(ListaIdentificadores *lista) {
    for (int i = 0; i < lista->longitud; ++i)
        borrar_identificador(&lista->valores[i]);
    free(lista->valores);
    lista->valores = NULL;
    lista->capacidad = 0;
    lista->longitud = 0;
}

void _imprimir_lista_identificadores(ListaIdentificadores listaIdentificadores) {
    for (int i = 0; i < listaIdentificadores.longitud; ++i) {
        printf("%s, ", string_a_puntero(&listaIdentificadores.valores[i].nombre));
    }
}

void imprimir_lista_identificadores(ListaIdentificadores listaIdentificadores) {
    _imprimir_lista_identificadores(listaIdentificadores);
    printf("\n");
}