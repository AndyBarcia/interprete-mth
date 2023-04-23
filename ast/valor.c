#include "valor.h"
#include "ast.h"
#include "../bibliotecas/biblioteca_dinamica.h"
#include <stdlib.h>
#include <stdio.h>

Valor crear_valor_unidad(Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = clonar_loc(*loc);
        loc = loc_copy;
    }
    return (Valor) {TIPO_UNIDAD, NULL, loc};
}

Valor crear_valor_indefinido() {
    return (Valor) {TIPO_INDEFINIDO, NULL, NULL};
}

Valor crear_entero(Entero entero, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = clonar_loc(*loc);
        loc = loc_copy;
    }
    return (Valor) {TIPO_ENTERO, NULL, loc, .entero = entero};
}

Valor crear_decimal(Decimal decimal, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = clonar_loc(*loc);
        loc = loc_copy;
    }
    return (Valor) {TIPO_DECIMAL, NULL, loc, .decimal = decimal};
}

Valor crear_bool(Bool bool, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = clonar_loc(*loc);
        loc = loc_copy;
    }
    return (Valor) {TIPO_BOOL, NULL, loc, .entero = bool ? 1 : 0};
}

Valor crear_valor_string(String string, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = clonar_loc(*loc);
        loc = loc_copy;
    }
    int *referencias = malloc(sizeof(int));
    *referencias = 1;
    return (Valor) {TIPO_STRING, referencias, loc, .string = string};
}

Valor crear_funcion_intrinseca(FuncionIntrinseca funcion, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = clonar_loc(*loc);
        loc = loc_copy;
    }
    return (Valor) {TIPO_FUNCION_INTRINSECA, NULL, loc, .funcion_intrinseca = funcion};
}

Valor crear_funcion(ListaIdentificadores argumentos, struct Expresion *cuerpo, struct TablaHash *capturadas, Localizacion *loc) {
    if (loc) {
        Localizacion* loc_copy = malloc(sizeof(Localizacion));
        *loc_copy = clonar_loc(*loc);
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
        *loc_copy = clonar_loc(*loc);
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
        *loc_copy = clonar_loc(*loc);
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
        *loc_copy = clonar_loc(*loc);
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

Valor extraer_valor_control_flujo(ValorControlFlujo *control_flujo) {
    if (control_flujo->valor) {
        Valor r = *(Valor*) control_flujo->valor;
        free(control_flujo->valor);
        control_flujo->valor = NULL;
        return r;
    } else {
        return crear_valor_unidad(NULL);
    }
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
        *copia.loc = clonar_loc(*v.loc);
    }
    return copia;
}

Valor clonar_valor_debil(Valor v) {
    Valor copia = v;
    copia.referencias = NULL;
    if (v.loc) {
        // Si no, clonar también la información de
        // localización del código fuente si fuese necesario.
        copia.loc = malloc(sizeof(Localizacion));
        *copia.loc = clonar_loc(*v.loc);
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
                borrar_loc(valor->loc);
                free(valor->loc);
                valor->loc = NULL;
            }
            // Restablecemos valores a su valor por defecto
            // y liberamos memoria.
            switch (valor->tipo_valor) {
                case TIPO_ENTERO: valor->entero = 0; break;
                case TIPO_DECIMAL: valor->decimal = 0; break;
                case TIPO_BOOL: valor->bool = 0; break;
                case TIPO_FUNCION_INTRINSECA: valor->funcion_intrinseca = -1; break;
                case TIPO_FUNCION_FORANEA: valor->funcion_foranea = NULL; break;
                case TIPO_ERROR: borrar_error(&valor->error); break;
                case TIPO_STRING: borrar_string(&valor->string); break;
                case TIPO_BIBLIOTECA_FORANEA: cerrar_biblioteca_dinamica(&valor->biblioteca); break;
                case TIPO_FUNCION:
                    borrar_lista_identificadores(&valor->funcion.nombres_args);
                    borrar_expresion((Expresion *) valor->funcion.cuerpo);
                    free(valor->funcion.cuerpo);
                    borrar_tabla_hash((TablaHash *) valor->funcion.variables_capturadas);
                    free(valor->funcion.variables_capturadas);
                    break;
                case TIPO_CONTROL_FLUJO:
                    if (valor->control_flujo.valor) {
                        borrar_valor((Valor *) valor->control_flujo.valor);
                        free(valor->control_flujo.valor);
                    }
                    break;
                default: break;
            }
            free(valor->referencias);
            valor->referencias = NULL;
        }
    } else {
        // Si no es un valor dinámico, entonces liberar la
        // memoria sobre localización en el código fuente, si
        // la había, y terminar.
        if (valor->loc) {
            borrar_loc(valor->loc);
            free(valor->loc);
            valor->loc = NULL;
        }
        // Restablece los valores de este valor a su valor por
        // defecto por si acaso, aunque no debería ser necesario.
        // Importante: no borramos valores dinámicos con free
        // porque este es un valor que no lleva cuenta de sus
        // referencias, por lo que no podemos saber si alguien
        // está utilizando la memoria.
        switch (valor->tipo_valor) {
            case TIPO_ENTERO: valor->entero = 0; break;
            case TIPO_DECIMAL: valor->decimal = 0; break;
            case TIPO_BOOL: valor->bool = 0; break;
            case TIPO_FUNCION_INTRINSECA: valor->funcion_intrinseca = -1; break;
            case TIPO_FUNCION_FORANEA: valor->funcion_foranea = NULL; break;
            case TIPO_BIBLIOTECA_FORANEA: valor->biblioteca = NULL; break;
            case TIPO_CONTROL_FLUJO: valor->control_flujo.valor = NULL; break;
            case TIPO_ERROR:
                valor->error.mensaje.puntero = 0;
                valor->error.mensaje.padding[0] = '\0';
                break;
            case TIPO_STRING:
                valor->string.puntero = 0;
                valor->string.padding[0] = '\0';
                break;
            case TIPO_FUNCION:
                valor->funcion.cuerpo = NULL;
                valor->funcion.variables_capturadas = NULL;
                valor->funcion.nombres_args.valores = NULL;
                break;
            default: break;
        }
    }
    valor->tipo_valor = TIPO_INDEFINIDO;
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
                default: return 1;
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
        case TIPO_INDEFINIDO:
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
            *resultado = strcmp(string_a_str(&a.string), string_a_str(&b.string));
            return 1;
        default:
            return 0;
    }
}

int acceder_miembro_valor(Valor v, Valor i, Valor *result) {
    switch (v.tipo_valor) {
        // En las bibliotecas dinámicas de C se puede acceder
        // a las funciones foráneas mediante strings, que
        // representan los nombres de las funciones.
        case TIPO_BIBLIOTECA_FORANEA: {
            switch (i.tipo_valor) {
                case TIPO_STRING: {
                    BibilotecaDinamica bib = v.biblioteca;
                    FuncionForanea f = cargar_funcion_biblioteca(bib, string_a_str(&i.string));
                    if (f) {
                        *result = crear_funcion_foranea(f);
                    } else {
                        Error error = crear_error("No existe la función foránea \"%s\" en la biblioteca.",
                                                  string_a_str(&i.string));
                        *result = crear_valor_error(error, i.loc);
                    }
                    return 1;
                }
                default: return 0;
            }
        }
        // En los strings se puede acceder a caracteres
        // del string mediante enteros.
        case TIPO_STRING: {
            switch (i.tipo_valor) {
                case TIPO_ENTERO: {
                    char* str = string_a_str(&v.string);
                    int len = (int) strlen(str);

                    int n = i.entero;
                    if (n < 0) n += len;

                    if (n < 0 || n >= len) {
                        Error error = crear_error("El valor %d está fuera de rango.", i.entero);
                        *result = crear_valor_error(error, i.loc);
                    } else {
                        char xs[2];
                        xs[0] = str[n];
                        xs[1] = '\0';

                        *result = crear_valor_string(crear_string(xs), NULL);
                    }
                    return 1;
                }
                default: return 0;
            }
        }
        default: return 0;
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
        *lista->loc = clonar_loc(*v.loc);
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
        borrar_loc(lista->loc);
        free(lista->loc);
        lista->loc = NULL;
    }
}

void _imprimir_error(Error error, Localizacion *loc);

char* _intrinseca_a_str(FuncionIntrinseca f) {
    switch (f) {
        case INTRINSECA_SUMA: return "+";
        case INTRINSECA_RESTA: return "-";
        case INTRINSECA_MULT: return "*";
        case INTRINSECA_DIV: return "/";
        case INTRINSECA_MOD: return "%";
        case INTRINSECA_EQ: return "==";
        case INTRINSECA_NEQ: return "!=";
        case INTRINSECA_GE: return ">";
        case INTRINSECA_GEQ: return ">=";
        case INTRINSECA_LE: return "<";
        case INTRINSECA_LEQ: return "<=";
        case INTRINSECA_AND: return "&&";
        case INTRINSECA_OR: return "||";
        case INTRINSECA_NOT: return "!";
        case INTRINSECA_NEGAR: return "-";
        case INTRINSECA_PRINT: return "print";
        case INTRINSECA_PRINTWS: return "printws";
        case INTRINSECA_RESETWS: return "resetws";
        case INTRINSECA_AYUDA: return "help";
        case INTRINSECA_CARGAR: return "load";
        case INTRINSECA_EVAL: return "eval";
        case INTRINSECA_EXIT: return "exit";
        case INTRINSECA_CALLFOREIGN: return "callforeign";
        case INTRINSECA_CAST_ENTERO: return "int";
        case INTRINSECA_CAST_DECIMAL: return "decimal";
    }
}

void _imprimir_valor(Valor valor) {
    switch (valor.tipo_valor) {
        case TIPO_UNIDAD: /*printf("unidad");*/ break;
        case TIPO_INDEFINIDO:
            printf("indefinido");
            break;
        case TIPO_ERROR:
            _imprimir_error(valor.error, valor.loc);
            break;
        case TIPO_FUNCION_INTRINSECA:
            printf("%s", _intrinseca_a_str(valor.funcion_intrinseca));
            break;
        case TIPO_FUNCION:
            printf("[función %d-aria]", valor.funcion.nombres_args.longitud);
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
            printf("%s", string_a_str(&valor.string));
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
    if (valor.tipo_valor != TIPO_UNIDAD) printf("\n");
}

char* tipo_valor_a_str(TipoValor tipo) {
    switch (tipo) {
        case TIPO_INDEFINIDO: return "indefinido";
        case TIPO_UNIDAD: return "unidad";
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
