#include "valor.h"
#include "ast.h"
#include <stdlib.h>
#include <stdio.h>

Valor crear_indefinido() {
    return (Valor) {TIPO_INDEFINIDO, NULL, NULL};
}

Valor crear_nulo() {
    return (Valor) {TIPO_NULO, NULL, NULL};
}

Valor crear_entero(Entero entero, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }
    return (Valor) {TIPO_ENTERO, NULL, loc, .entero = entero};
}

Valor crear_decimal(Decimal decimal, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }
    return (Valor) {TIPO_DECIMAL, NULL, loc, .decimal = decimal};
}

Valor crear_bool(Bool bool, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }
    return (Valor) {TIPO_BOOL, NULL, loc, .entero = bool ? 1 : 0};
}

Valor crear_valor_string(String string, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }
    int *referencias = malloc(sizeof(int));
    *referencias = 1;
    return (Valor) {TIPO_STRING, referencias, loc, .string = string};
}

Valor crear_funcion_intrinseca(FuncionIntrinseca funcion, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }
    return (Valor) {TIPO_FUNCION_INTRINSECA, NULL, loc, .funcion_intrinseca = funcion};
}

Valor crear_funcion(ListaIdentificadores argumentos, struct Expresion *cuerpo, struct TablaHash *capturadas, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }
    int *referencias = malloc(sizeof(int));
    *referencias = 1;

    return (Valor) {
            .tipo_valor = TIPO_FUNCION,
            .referencias = referencias,
            .loc = loc,
            .funcion = (Funcion) {
                    .nombres_args = argumentos,
                    .cuerpo = cuerpo,
                    .variables_capturadas = capturadas
            }
    };
}

Valor crear_funcion_foranea(FuncionForanea foranea) {
    return (Valor) {
        .tipo_valor = TIPO_FUNCION_FORANEA,
        .referencias = NULL,
        .loc = NULL,
        .funcion_foranea = foranea
    };
}

Valor crear_valor_biblioteca(BibilotecaDinamica biblioteca, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }
    int *referencias = malloc(sizeof(int));
    *referencias = 1;
    return (Valor) {
        .tipo_valor = TIPO_BIBLIOTECA_FORANEA,
        .referencias = referencias,
        .loc = loc,
        .biblioteca = biblioteca
    };
}

Valor crear_valor_control_flujo(TipoControlFlujo tipo, Valor *v, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }
    int *referencias = malloc(sizeof(int));
    *referencias = 1;

    if (v) {
        Valor *vp = malloc(sizeof(Valor));
        *vp = *v;
        v = vp;
    }

    return (Valor) {
            .tipo_valor = TIPO_CONTROL_FLUJO,
            .referencias = referencias,
            .loc = loc,
            .control_flujo = (ValorControlFlujo) {
                .tipo = tipo,
                .valor = (struct Valor*) v
            }
    };
}

Valor crear_valor_error(Error error, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = *loc;
        loc = loc_copy;
    }
    int *referencias = malloc(sizeof(int));
    *referencias = 1;
    return (Valor) {
            .tipo_valor = TIPO_ERROR,
            .referencias = referencias,
            .loc = loc,
            .error = error
    };
}

Valor clonar_valor(Valor v) {
    Valor copia = v;
    // Al clonar aumentar el número de referencias si es un
    // valor dinámico (string, error, función, etc).
    if (copia.referencias) {
        *copia.referencias += 1;
    } else if (v.loc) {
        // Si no, clonar también la información de
        // localización del código fuente si fuese necesario.
        copia.loc = malloc(sizeof(Localizacion));
        *copia.loc = *v.loc;
    }
    return copia;
}

void borrar_valor(Valor *valor) {
    // Si es un valor dinámico (string, error, función, etc),
    // reducir el número de referencias dinámicas.
    if (valor->referencias) {
        *valor->referencias -= 1;
        
        // Si las referencias llegan a 0, liberar la memoria.
        if (*valor->referencias <= 0) {
            if (valor->loc) {
                free(valor->loc);
                valor->loc = NULL;
            }
            switch (valor->tipo_valor) {
                case TIPO_ERROR:
                    borrar_error(&valor->error);
                    break;
                case TIPO_STRING:
                    borrar_string(&valor->string);
                    break;
                case TIPO_FUNCION:
                    borrar_lista_identificadores(&valor->funcion.nombres_args);
                    borrar_expresion((Expresion *) valor->funcion.cuerpo);
                    free(valor->funcion.cuerpo);
                    borrar_tabla_hash((TablaHash *) valor->funcion.variables_capturadas);
                    free(valor->funcion.variables_capturadas);
                    break;
                case TIPO_BIBLIOTECA_FORANEA:
                    cerrar_biblioteca_dinamica(&valor->biblioteca);
                case TIPO_CONTROL_FLUJO:
                    if (valor->control_flujo.valor) {
                        borrar_valor((Valor *) valor->control_flujo.valor);
                        free(valor->control_flujo.valor);
                    }
                default:
                    break;
            }
            free(valor->referencias);
            valor->referencias = NULL;
        }
    } else {
        // Si no es un valor dinámico, entonces liberar la
        // memoria sobre localización en el código fuente, si
        // la había , y terminar.
        if (valor->loc) {
            free(valor->loc);
            valor->loc = NULL;
        }
        switch (valor->tipo_valor) {
            case TIPO_ENTERO: valor->entero = 0; break;
            case TIPO_DECIMAL: valor->decimal = 0; break;
            case TIPO_BOOL: valor->bool = 0; break;
            case TIPO_FUNCION_INTRINSECA: valor->funcion_intrinseca = -1; break;
            case TIPO_FUNCION_FORANEA: valor->funcion_foranea = NULL; break;
            default: break;
        }
    }
    valor->tipo_valor = TIPO_NULO;
}

int comparar_valor(Valor a, Valor b, int *resultado) {
    switch (a.tipo_valor) {
        case TIPO_ENTERO:
            switch (b.tipo_valor) {
                case TIPO_ENTERO:
                    *resultado =  a.entero - b.entero;
                    return 1;
                case TIPO_DECIMAL: {
                    double dif = ((double) a.entero) - b.decimal;
                    if (dif > 0.0) *resultado = 1;
                    else if (dif < 0.0) *resultado = -1;
                    else *resultado = 0;
                    return 1;
                }
                default: return 0;
            }
        case TIPO_DECIMAL:
            switch (b.tipo_valor) {
                case TIPO_ENTERO: {
                    double dif = a.decimal - ((double) b.decimal);
                    if (dif > 0.0) *resultado = 1;
                    else if (dif < 0.0) *resultado = -1;
                    else *resultado = 0;
                    return 1;
                }
                case TIPO_DECIMAL: {
                    double dif = a.decimal - b.decimal;
                    if (dif > 0.0) *resultado = 1;
                    else if (dif < 0.0) *resultado = -1;
                    else *resultado = 0;
                    return 1;
                }
                default: return 0;
            }
        case TIPO_NULO:
            if (a.tipo_valor != b.tipo_valor) return 0;
            *resultado = 0;
            return 1;
        case TIPO_FUNCION_INTRINSECA:
            if (a.tipo_valor != b.tipo_valor) return 0;
            *resultado = a.funcion_intrinseca != b.funcion_intrinseca;
            return 1;
        case TIPO_BOOL:
            if (a.tipo_valor != b.tipo_valor) return 0;
            *resultado =  a.bool - b.bool;
            return 1;
        case TIPO_STRING:
            if (a.tipo_valor != b.tipo_valor) return 0;
            *resultado = strcmp(string_a_puntero(&a.string), string_a_puntero(&b.string));
            return 1;
        default:
            return 0;
    }
}

ListaValores crear_lista_valores() {
    return (ListaValores) {
        .capacidad = 0,
        .longitud = 0,
        .valores = NULL,
        .loc = NULL
    };
}

void push_lista_valores(ListaValores *lista, Valor v) {
    if (lista->longitud >= lista->capacidad) {
        lista->valores = realloc(lista->valores, (lista->capacidad + 1) * sizeof(Valor));
        ++lista->capacidad;
    }
    lista->valores[lista->longitud++] = v;
    if (lista->loc == NULL && v.loc) {
        lista->loc = malloc(sizeof(Localizacion));
        *lista->loc = *v.loc;
    } else if (v.loc) {
        lista->loc->last_line = v.loc->last_line;
        lista->loc->last_column = v.loc->last_column;
    }
}

void borrar_lista_valores(ListaValores *lista) {
    for (int i = 0; i < lista->longitud; ++i)
        borrar_valor(&((Valor *) lista->valores)[i]);
    free(lista->valores);
    if (lista->loc) {
        free(lista->loc);
        lista->loc = NULL;
    }
}

void _imprimir_valor(Valor valor) {
    switch (valor.tipo_valor) {
        case TIPO_INDEFINIDO: /*printf("indefinido");*/ break;
        case TIPO_NULO:
            printf("null");
            break;
        case TIPO_ERROR:
            printf("%s", string_a_puntero(&valor.error.mensaje));
            break;
        case TIPO_FUNCION_INTRINSECA:
            printf("[función intrínseca]");
            break;
        case TIPO_FUNCION:
            printf("[función]");
            break;
        case TIPO_FUNCION_FORANEA:
            printf("[función foránea]");
            break;
        case TIPO_ENTERO:
            printf("%d", valor.entero);
            break;
        case TIPO_DECIMAL:
            printf("%f", valor.decimal);
            break;
        case TIPO_BOOL:
            printf("%s", valor.bool ? "Verdadero" : "Falso");
            break;
        case TIPO_STRING:
            printf("%s", string_a_puntero(&valor.string));
            break;
        case TIPO_BIBLIOTECA_FORANEA:
            printf("[biblioteca]");
            break;
        case TIPO_CONTROL_FLUJO:
            printf("[%s", tipo_control_flujo_a_str(valor.control_flujo.tipo));
            if (valor.control_flujo.valor) {
                printf(" ");
                _imprimir_valor(*(Valor*) valor.control_flujo.valor);
                printf(" ");
            }
            printf("]");
            break;
    }
}

void imprimir_valor(Valor valor) {
    _imprimir_valor(valor);
    if (valor.tipo_valor != TIPO_INDEFINIDO) printf("\n");
}

char* tipo_valor_a_str(TipoValor tipo) {
    switch (tipo) {
        case TIPO_NULO: return "nulo";
        case TIPO_INDEFINIDO: return "indefinido";
        case TIPO_ERROR: return "error";
        case TIPO_ENTERO: return "entero";
        case TIPO_DECIMAL: return "decimal";
        case TIPO_BOOL: return "booleano";
        case TIPO_STRING: return "string";
        case TIPO_FUNCION_INTRINSECA: return "función intrínseca";
        case TIPO_FUNCION_FORANEA: return "función foránea";
        case TIPO_FUNCION: return "función";
        case TIPO_BIBLIOTECA_FORANEA: return "biblioteca";
        case TIPO_CONTROL_FLUJO: return "control de flujo";
    }
}
