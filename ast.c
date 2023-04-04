#include "ast.h"
#include "tabla_simbolos.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

Valor crear_indefinido() {
    return (Valor) {TIPO_INDEFINIDO, NULL};
}

Valor crear_nulo() {
    return (Valor) {TIPO_NULO, NULL};
}

Valor crear_entero(Entero entero) {
    return (Valor) {TIPO_ENTERO, NULL, .entero = entero};
}

Valor crear_bool(Bool bool) {
    return (Valor) {TIPO_BOOL, NULL, .entero = bool ? 1 : 0};
}

Valor crear_valor_string(String string) {
    int *referencias = malloc(sizeof(int));
    *referencias = 1;
    return (Valor) {TIPO_STRING, referencias, .string = string};
}

Valor crear_funcion_nativa(FuncionNativa funcion) {
    return (Valor) {TIPO_FUNCION_NATIVA, NULL, .funcion_nativa = funcion};
}

Valor crear_funcion(ListaIdentificadores argumentos, Expresion *cuerpo, struct TablaHash *capturadas) {
    int *referencias = malloc(sizeof(int));
    *referencias = 1;

    return (Valor) {
            .tipoValor = TIPO_FUNCION,
            .referencias = referencias,
            .funcion = (Funcion) {
                    .argumentos = argumentos,
                    .cuerpo = (struct Expresion *) cuerpo,
                    .variables_capturadas = capturadas
            }
    };
}

Valor clonar_valor(Valor v) {
    Valor copia = v;
    // Al clonar aumentar el número de referencias si es un
    // valor dinámico (string, error, función, etc).
    if (copia.referencias)
        *copia.referencias += 1;
    return copia;
}

void borrar_valor(Valor *valor) {
    // Si es un valor dinámico (string, error, función, etc),
    // reducir el número de referencias dinámicas; y si estas
    // legan a 0, liberar la memoria.
    if (valor->referencias) {
        *valor->referencias -= 1;
        if (*valor->referencias <= 0) {
            switch (valor->tipoValor) {
                case TIPO_ERROR:
                    borrar_string(&valor->error); break;
                case TIPO_STRING:
                    borrar_string(&valor->string); break;
                case TIPO_FUNCION:
                    borrar_lista_identificadores(&valor->funcion.argumentos);
                    borrar_expresion((Expresion*) valor->funcion.cuerpo);
                    free(valor->funcion.cuerpo);
                    borrar_tabla_hash((TablaHash*) valor->funcion.variables_capturadas);
                    free(valor->funcion.variables_capturadas);
                    break;
            }
            free(valor->referencias);
            valor->referencias = NULL;
        }
    }
    valor->tipoValor = TIPO_INDEFINIDO;
}

void borrar_lista_valores(ListaValores *lista) {
    for (int i = 0; i < lista->longitud; ++i)
        borrar_valor(&((Valor*)lista->valores)[i]);
    free(lista->valores);
}

Valor crear_error(const char *formato, ...) {
    String error;
    // Cosas mágicas para pasar un número variable de argumentos.
    va_list args, args2;
    va_start(args, formato);
    va_copy(args2, args);
    // Calcular la longitud total que tendrá el string final.
    size_t longitud = vsnprintf(NULL, 0, formato, args);
    error = crear_string_n(longitud);
    // Escribir el resultado en el string creado.
    char *str = string_a_puntero(&error);
    vsnprintf(str, longitud + 1, formato, args2);
    va_end(args);
    va_end(args2);

    return (Valor) {
            .tipoValor = TIPO_ERROR,
            .error = error
    };
}

void _imprimir_valor(Valor valor) {
    switch (valor.tipoValor) {
        case TIPO_INDEFINIDO: /*printf("indefinido");*/ break;
        case TIPO_NULO:
            printf("null");
            break;
        case TIPO_ERROR:
            printf("Error: %s", string_a_puntero(&valor.error));
            break;
        case TIPO_FUNCION_NATIVA:
            printf("[función nativa]");
            break;
        case TIPO_FUNCION:
            printf("[función]");
            break;
        case TIPO_ENTERO:
            printf("%d", valor.entero);
            break;
        case TIPO_BOOL:
            printf("%s", valor.bool ? "Verdadero" : "Falso");
            break;
        case TIPO_STRING:
            printf("%s", string_a_puntero(&valor.string));
            break;
    }
}

void imprimir_valor(Valor valor) {
    _imprimir_valor(valor);
    if (valor.tipoValor != TIPO_INDEFINIDO) printf("\n");
}

int comparar_valor(Valor a, Valor b) {
    if (a.tipoValor != b.tipoValor) return 0;
    switch (a.tipoValor) {
        case TIPO_INDEFINIDO:
            return 0;
        case TIPO_NULO:
            return 1;
        case TIPO_ERROR:
            return 1;
        case TIPO_FUNCION_NATIVA:
            return a.funcion_nativa == b.funcion_nativa;
        case TIPO_FUNCION:
            return 0;
        case TIPO_ENTERO:
            return a.entero == b.entero;
        case TIPO_BOOL:
            return a.bool == b.bool;
        case TIPO_STRING:
            return strcmp(string_a_puntero(&a.string), string_a_puntero(&b.string)) == 0;
        default:
            return 0;
    }
}

void _imprimir_lista_identificadores(ListaIdentificadores listaIdentificadores) {
    for (int i = 0; i < listaIdentificadores.longitud; ++i) {
        printf("%s, ", string_a_puntero(&listaIdentificadores.valores[i]));
    }
}

void imprimir_lista_identificadores(ListaIdentificadores listaIdentificadores) {
    _imprimir_lista_identificadores(listaIdentificadores);
    printf("\n");
}

void _imprimir_lista_expresiones(ListaExpresiones listaValores);

void _imprimir_expresion(Expresion expresion) {
    switch (expresion.tipo) {
        case EXP_VALOR:
            _imprimir_valor(expresion.valor);
            break;
        case EXP_IDENTIFICADOR:
            printf("%s", string_a_puntero(&expresion.identificador));
            break;
        case EXP_OP_LLAMADA:
            _imprimir_expresion(*(Expresion *) expresion.llamadaFuncion.funcion);
            _imprimir_lista_expresiones(expresion.llamadaFuncion.argumentos);
            break;
        case EXP_OP_ASIGNACION:
            printf("%s = ", string_a_puntero(&expresion.asignacion.identificador));
            _imprimir_expresion(*(Expresion *) expresion.asignacion.expresion);
            break;
        case EXP_OP_DEF_FUNCION:
            printf("\\");
            _imprimir_lista_identificadores(expresion.defFuncion.argumentos);
            printf(" => ");
            _imprimir_expresion(*(Expresion *) expresion.defFuncion.cuerpo);
            break;
    }
}

void imprimir_expresion(Expresion expresion) {
    _imprimir_expresion(expresion);
    printf("\n");
}

void _imprimir_lista_expresiones(ListaExpresiones listaExpresiones) {
    printf("[");
    for (int i = 0; i < listaExpresiones.longitud; ++i) {
        _imprimir_expresion(((Expresion *) listaExpresiones.valores)[i]);
        printf(", ");
    }
    printf("]");
}

void imprimir_lista_expresiones(ListaExpresiones listaExpresiones) {
    _imprimir_lista_expresiones(listaExpresiones);
    printf("\n");
}

void _variables_capturadas(Expresion expresion, TablaHash *locales, ListaIdentificadores *lista) {
    switch (expresion.tipo) {
        case EXP_VALOR:
        case EXP_NULA:
            break;
        case EXP_IDENTIFICADOR:
            if (!es_miembro_hash(*locales, string_a_puntero(&expresion.identificador)))
                push_lista_identificadores(lista, expresion.identificador);
            break;
        case EXP_OP_LLAMADA:
            _variables_capturadas(*(Expresion *) expresion.llamadaFuncion.funcion, locales, lista);
            for (int i = 0; i < expresion.llamadaFuncion.argumentos.longitud; ++i)
                _variables_capturadas(((Expresion *) expresion.llamadaFuncion.argumentos.valores)[i], locales, lista);
            break;
        case EXP_OP_ASIGNACION:
            insertar_hash(locales, expresion.asignacion.identificador, crear_indefinido(), 0);
            _variables_capturadas(*(Expresion *) expresion.asignacion.expresion, locales, lista);
            break;
        case EXP_OP_DEF_FUNCION:
            for (int i = 0; i < expresion.defFuncion.argumentos.longitud; ++i)
                insertar_hash(locales, expresion.defFuncion.argumentos.valores[i], crear_indefinido(), 0);
            _variables_capturadas(*(Expresion *) expresion.defFuncion.cuerpo, locales, lista);
            break;
        case EXP_BLOQUE:
            for (int i = 0; i < expresion.bloque.longitud; ++i) {
                Expresion subexpresion = ((Expresion *) expresion.bloque.valores)[i];
                _variables_capturadas(subexpresion, locales, lista);
            }
            break;
    }
}

ListaIdentificadores variables_capturadas(DefinicionFuncion funcion) {
    ListaIdentificadores capturadas = crear_lista_identificadores();
    TablaHash locales = crear_tabla_hash(funcion.argumentos.longitud);
    for (int i = 0; i < funcion.argumentos.longitud; ++i)
        insertar_hash(&locales, funcion.argumentos.valores[i], crear_indefinido(), 0);

    Expresion cuerpo = *(Expresion *) funcion.cuerpo;
    _variables_capturadas(cuerpo, &locales, &capturadas);
    borrar_tabla_hash(&locales);
    return capturadas;
}

Expresion crear_exp_nula() {
    return (Expresion) {EXP_NULA};
}

Expresion crear_exp_valor(Valor valor) {
    return (Expresion) {EXP_VALOR, .valor = valor, .es_sentencia = 0};
}

Expresion crear_exp_identificador(String identificador) {
    return (Expresion) {EXP_IDENTIFICADOR, .identificador = identificador, .es_sentencia = 0};
}

Expresion crear_exp_llamada(Expresion funcion, ListaExpresiones argumentos) {
    Expresion *e = malloc(sizeof(Expresion));
    *e = funcion;

    return (Expresion) {
            .tipo = EXP_OP_LLAMADA,
            .llamadaFuncion = (LlamadaFuncion) {
                    .funcion = (struct Expresion *) e,
                    .argumentos = argumentos
            },
            .es_sentencia = 0,
    };
}

Expresion crear_exp_op_unaria(String operador, Expresion x) {
    ListaExpresiones args = crear_lista_expresiones();
    push_lista_expresiones(&args, x);
    return crear_exp_llamada(crear_exp_identificador(operador), args);
}

Expresion crear_exp_op_binaria(String operador, Expresion a, Expresion b) {
    ListaExpresiones args = crear_lista_expresiones();
    push_lista_expresiones(&args, a);
    push_lista_expresiones(&args, b);
    return crear_exp_llamada(crear_exp_identificador(operador), args);
}

Expresion crear_exp_asignacion(String identificador, Expresion expresion, int inmutable) {
    Expresion *e = malloc(sizeof(Expresion));
    *e = expresion;

    return (Expresion) {
            .tipo = EXP_OP_ASIGNACION,
            .asignacion = (Asignacion) {
                    .identificador = identificador,
                    .expresion = (struct Expresion *) e,
                    .inmutable = inmutable
            },
            .es_sentencia = 0,
    };
}

Expresion crear_exp_def_funcion(ListaIdentificadores argumentos, Expresion cuerpo) {
    Expresion *e = malloc(sizeof(Expresion));
    *e = cuerpo;

    return (Expresion) {
            .tipo = EXP_OP_DEF_FUNCION,
            .defFuncion = (DefinicionFuncion) {
                    .argumentos = argumentos,
                    .cuerpo = (struct Expresion *) e
            },
            .es_sentencia = 0,
    };
}

Expresion crear_exp_bloque(ListaExpresiones expresiones) {
    return (Expresion) {
            .tipo = EXP_BLOQUE,
            .bloque = expresiones,
            .es_sentencia = 0,
    };
}

Expresion clonar_expresion(Expresion exp) {
    Expresion e = exp;
    switch (e.tipo) {
        case EXP_NULA: break;
        case EXP_VALOR:
            e.valor = clonar_valor(exp.valor);
            break;
        case EXP_IDENTIFICADOR:
            e.identificador = clonar_string(exp.identificador);
            break;
        case EXP_OP_LLAMADA:
            e.llamadaFuncion.funcion = malloc(sizeof(Expresion));
            *(Expresion*)e.llamadaFuncion.funcion = clonar_expresion(*(Expresion*)exp.llamadaFuncion.funcion);
            e.llamadaFuncion.argumentos = clonar_lista_expresiones(exp.llamadaFuncion.argumentos);
            break;
        case EXP_OP_ASIGNACION:
            e.asignacion.identificador = clonar_string(exp.asignacion.identificador);
            *(Expresion*)e.asignacion.expresion = clonar_expresion(*(Expresion*)exp.asignacion.expresion);
            break;
        case EXP_OP_DEF_FUNCION:
            e.defFuncion.argumentos = clonar_lista_identificadores(exp.defFuncion.argumentos);
            *(Expresion*)e.defFuncion.cuerpo = clonar_expresion(*(Expresion*)exp.defFuncion.cuerpo);
            break;
        case EXP_BLOQUE:
            e.bloque = clonar_lista_expresiones(exp.bloque);
            break;
    }
    return e;
}

void borrar_expresion(Expresion *exp) {
    switch (exp->tipo) {
        case EXP_NULA: break;
        case EXP_VALOR:
            borrar_valor(&exp->valor);
            break;
        case EXP_IDENTIFICADOR:
            borrar_string(&exp->identificador);
            break;
        case EXP_OP_LLAMADA:
            borrar_expresion((Expresion*) exp->llamadaFuncion.funcion);
            free(exp->llamadaFuncion.funcion);
            borrar_lista_expresiones(&exp->llamadaFuncion.argumentos);
            break;
        case EXP_OP_ASIGNACION:
            borrar_expresion((Expresion*) exp->asignacion.expresion);
            free(exp->asignacion.expresion);
            borrar_string(&exp->asignacion.identificador);
            break;
        case EXP_OP_DEF_FUNCION:
            borrar_expresion((Expresion*) exp->defFuncion.cuerpo);
            free(exp->defFuncion.cuerpo);
            borrar_lista_identificadores(&exp->defFuncion.argumentos);
            break;
        case EXP_BLOQUE:
            borrar_lista_expresiones(&exp->bloque);
            break;
    }
    exp->tipo = EXP_NULA;
}

ListaExpresiones crear_lista_expresiones() {
    return (ListaExpresiones) {
            .longitud = 0,
            .capacidad = 0,
            .valores = NULL
    };
}

void push_lista_expresiones(ListaExpresiones *lista, Expresion expresion) {
    if (lista->longitud >= lista->capacidad) {
        lista->valores = realloc(lista->valores, (lista->capacidad + 1) * sizeof(Expresion));
        ++lista->capacidad;
    }
    ((Expresion *) lista->valores)[lista->longitud++] = expresion;
}

ListaExpresiones clonar_lista_expresiones(ListaExpresiones lista) {
    ListaExpresiones result = {
            .capacidad = lista.longitud,
            .longitud = lista.longitud,
            .valores = malloc(sizeof(Expresion)*lista.longitud)
    };
    for (int i = 0; i < lista.longitud; ++i)
        ((Expresion*)result.valores)[i] = clonar_expresion(((Expresion*)lista.valores)[i]);
    return result;
}

void borrar_lista_expresiones(ListaExpresiones *lista) {
    for (int i = 0; i < lista->longitud; ++i)
        borrar_expresion(&((Expresion*) lista->valores)[i]);
    free(lista->valores);
    lista->capacidad = 0;
    lista->longitud = 0;
}

void borrar_lista_identificadores(ListaIdentificadores *lista) {
    for (int i = 0; i < lista->longitud; ++i)
        borrar_string(&lista->valores[i]);
    free(lista->valores);
    lista->capacidad = 0;
    lista->longitud = 0;
}

ListaIdentificadores crear_lista_identificadores() {
    return (ListaIdentificadores) {
            .longitud = 0,
            .capacidad = 0,
            .valores = NULL
    };
}

void push_lista_identificadores(ListaIdentificadores *lista, String identificador) {
    if (lista->longitud >= lista->capacidad) {
        lista->valores = realloc(lista->valores, (lista->capacidad + 1) * sizeof(String));
        ++lista->capacidad;
    }
    ((String *) lista->valores)[lista->longitud++] = identificador;
}

ListaIdentificadores clonar_lista_identificadores(ListaIdentificadores lista) {
    ListaIdentificadores copia = (ListaIdentificadores) {
        .longitud = lista.longitud,
        .capacidad = lista.longitud,
        .valores = malloc(sizeof(String)*lista.longitud)
    };
    for (int i = 0; i < lista.longitud; ++i)
        copia.valores[i] = clonar_string(lista.valores[i]);
    return copia;
}