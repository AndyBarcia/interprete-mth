#include "evaluador.h"
#include "analizador_sintactico.h"
#include "analizador_lexico.h"
#include "std.h"

Parser crear_parser(Lexer lexer, ContextoParsing contexto) {
    return (Parser) {
        .lexer = lexer,
        .ps = yypstate_new(),
        .contexto = contexto
    };
}

void borrar_parser(Parser *parser) {
    yypstate_delete((yypstate*) parser->ps);
    borrar_analizador_lexico(&parser->lexer);
}

int siguiente_expresion(Parser *parser, Expresion *expresion) {
    ContextoExpresion ctx = {
            .es_modulo = parser->contexto == CONTEXTO_MODULO,
            .es_bucle = 0,
            .es_asignacion = 0,
            .es_funcion = 0,
            .es_bloque = 0
    };

    int status;
    do {
        YYSTYPE token;
        int c = siguiente_componente_lexico(&parser->lexer, &token);

        Expresion exp = crear_exp_nula();
        status = yypush_parse((yypstate*) parser->ps, c, &token, (YYLTYPE*) &parser->lexer.loc, &exp);

        if (exp.tipo != EXP_NULA) {
            validar_expresion(&exp, ctx);
            *expresion = exp;
            return 1;
        }
    } while (status == YYPUSH_MORE);
    return 0;
}

Evaluador crear_evaluador(TablaSimbolos *tabla_simbolos, String wd) {
    return (Evaluador) {
        .tabla_simbolos = tabla_simbolos,
        .wd = wd,
        .debug = 0,
    };
}

void borrar_evaluador(Evaluador *evaluador) {
    borrar_string(&evaluador->wd);
}

// La expresión es un valor; devolverlo si no es una sentencia.
Valor evaluar_expresion_valor(ExpValor *exp, int es_sentencia) {
    if (!es_sentencia || exp->tipo_valor == TIPO_ERROR)
        return clonar_valor(*exp);
    else
        return crear_valor_unidad(NULL);
}

// La expresión representa el nombre de un lugar, como una variable
// `x` o un índice `x.y` o `x[y]`.
Valor evaluar_expresion_nombre(ExpNombre *exp, int es_sentencia, Evaluador *evaluador) {
    // Primero tenemos que encontrar el nombre base en la tabla de símbolos,
    // reportando en el caso de que no exista la variable.
    Valor valor = recuperar_clon_valor_tabla(*evaluador->tabla_simbolos, exp->nombre_base);
    if (valor.tipo_valor == TIPO_ERROR) return valor;

    // Y luego buscar recursivamente los accesos.
    for (int i = 0; i < exp->longitud; ++i) {
        Acceso acceso = exp->accesos[i];

        // Primero evaluamos el índice que vamos a utilizar
        Valor indice;
        switch (acceso.tipo) {
            case ACCESO_MIEMBRO:
                indice = crear_valor_string(acceso.miembro.nombre, &acceso.miembro.loc);
                break;
            case ACCESO_INDEXADO:
                indice = evaluar_expresion(evaluador, (Expresion*) acceso.indice);
                if (indice.tipo_valor == TIPO_ERROR) {
                    borrar_valor(&valor);
                    return indice;
                }
                break;
        }

        // Después intentamos acceder al miembro del valor,
        // reportando el caso de que estemos intentando
        // hacer algo imposible.
        Valor miembro;
        if (!acceder_miembro_valor(valor, indice, &miembro)) {
            Error error = crear_error_acceso_incorrecto(valor.tipo_valor, indice.tipo_valor);
            return crear_valor_error(error, indice.loc);
        }

        borrar_valor(&valor);
        borrar_valor(&indice);
        valor = miembro;
    }

    // Si la expresión es una sentencia, simplemente terminar,
    // porque no tenemos que devolver el valor.
    if (es_sentencia) {
        borrar_valor(&valor);
        return crear_valor_unidad(NULL);
    }

    // Si no, reemplazar la localización original del valor por la localización
    // del nombre asignable, y devolver el valor obtenido.
    if (valor.loc)
        borrar_loc(valor.loc);
    else
        valor.loc = malloc(sizeof(Localizacion));
    *valor.loc = clonar_loc(exp->nombre_base.loc);

    return valor;
}

ListaValores evaluar_expresiones(Evaluador *evaluador, ListaExpresiones *listaExpresiones);

Valor evaluar_expresion_llamada(ExpLlamada *exp, int es_sentencia, Evaluador *evaluador) {
    // Si es una llamada, evaluar la expresión que queremos llamar como función,
    // reportando el caso de que se haya producido un error.
    Valor f = evaluar_expresion(evaluador, (Expresion*)exp->funcion);
    if (f.tipo_valor == TIPO_ERROR) return f;

    // Obtener la lista de argumentos y abortar si alguno de ellos es un error.
    ListaValores args = evaluar_expresiones(evaluador, &exp->args);
    for(int i = 0; i < args.longitud; ++i) {
        Valor v = ((Valor*)args.valores)[i];
        if (v.tipo_valor == TIPO_ERROR) {
            Valor r = clonar_valor(v);
            borrar_lista_valores(&args);
            borrar_valor(&f);
            return r;
        }
    }

    // Tenemos que actuar de forma distinta en base a si es una
    // función intrínseca o una función definida por el usuario.
    switch (f.tipo_valor) {
        case TIPO_FUNCION_INTRINSECA: {
            // Una función intrínseca se ejecuta simplemente pasando el
            // tipo de función junto a los argumentos y la tabla de símbolos.

            Valor result = ejecutar_funcion_intrinseca(f.funcion_intrinseca, args, evaluador);
            borrar_valor(&f);

            if (es_sentencia) {
                borrar_valor(&result);
                return crear_valor_unidad(NULL);
            }
            return result;
        }
        case TIPO_FUNCION: {
            // Para una función definida por el usuario ya es más complicado.
            // Tenemos que comprobar que el número de argumentos es correcto;
            // introducirlos en la tabla de símbolos, junto con las variables
            // capturadas, y evaluar el cuerpo de la función.

            Funcion fn = f.funcion;

            if (fn.nombres_args.longitud != args.longitud) {
                Error error = crear_error_numero_argumentos(fn.nombres_args.longitud, fn.nombres_args.longitud, args.longitud);
                Valor v = crear_valor_error(error, exp->loc);
                borrar_lista_valores(&args);
                borrar_valor(&f);
                return v;
            }

            // Establecer una barrera para que las modificaciones de
            // variables dentro de la función no se propaguen fuera del
            // cuerpo de la función.
            aumentar_nivel_tabla_simbolos(evaluador->tabla_simbolos);
            establecer_barrera_tabla_simbolos(evaluador->tabla_simbolos);

            // Introducir los argumentos de la función en la tabla de símbolos
            for (int i = 0; i < args.longitud; ++i) {
                String nombre = clonar_string(fn.nombres_args.valores[i].nombre);

                TablaSimbolos *t = evaluador->tabla_simbolos;
                insertar_hash(&t->tablas[t->nivel], nombre, args.valores[i], 0);
            }
            free(args.valores);
            if (args.loc) {
                borrar_loc(args.loc);
                free(args.loc);
            }
            // Introducir las variables capturadas por la función
            asignar_clones_valores_tabla(evaluador->tabla_simbolos, *(TablaHash *) fn.variables_capturadas);

            // Evaluar la propia función.
            Valor v = evaluar_expresion(evaluador, (Expresion*)fn.cuerpo);

            // Si se devolvió un valor de control de flujo, tenemos que
            // sacar el valor del elemento de control (o propagarlo en el
            // caso de ser un control de flujo tipo exit).
            if (v.tipo_valor == TIPO_CONTROL_FLUJO) {
                switch (v.control_flujo.tipo) {
                    case CTR_FLUJO_EXIT: break;
                    default: {
                        Valor r = extraer_valor_control_flujo(&v.control_flujo);
                        borrar_valor(&v);
                        v = r;
                    }
                }
            }

            // Una vez terminado reducir nuevamente el nivel de la tabla
            // y libera la memoria.
            reducir_nivel_tabla_simbolos(evaluador->tabla_simbolos);
            borrar_valor(&f);

            if (es_sentencia) {
                borrar_valor(&v);
                return crear_valor_unidad(NULL);
            }
            return v;
        }
        case TIPO_FUNCION_FORANEA: {
            // Se está intentando llamar a una función foranea directamente
            // sin utilizar `callforeign`.
            return crear_valor_error(crear_error_llamada_foranea(), f.loc);
        }
        default: {
            return crear_valor_error(crear_error("Este valor no es una función!"), f.loc);
        }
    }
}

// Expresión de asignación de una expresión a una variable.
Valor evaluar_expresion_asignacion(ExpAsignacion *exp, int es_sentencia, Evaluador *evaluador) {
    String nombre = exp->nombre.nombre_base.nombre;

    // Comportamiento especial cuando estamos asignando una definición
    // de función a una variable. Esto permite que se pueda utilizar
    // recursividad del estilo de `f = \x => f(x-1)`, ya que de forma
    // normal esto causaría un error "variable f no definida" en el
    // cuerpo de la función.
    if (((Expresion*)exp->expresion)->tipo == EXP_OP_DEF_FUNCION) {
        //printf("%s\n", string_a_str(&nombre));
        // Asignar un valor especial indefinido.
        TipoAsignacion t = exp->tipo != ASIGNACION_EXPORT ? ASIGNACION_NORMAL : ASIGNACION_EXPORT;
        asignar_valor_tabla(evaluador->tabla_simbolos, clonar_string(nombre), crear_valor_indefinido(), t);
    }

    // Evaluar el valor que se va a asignar.
    Valor v = evaluar_expresion(evaluador, (Expresion*)exp->expresion);

    if (v.tipo_valor != TIPO_ERROR) {
        // Si la asignación es una sentencia, simplemente devolvemos valor unidad.
        // Si no, devolvemos un clon de valor que vamos a insertar en la tabla.
        Valor retorno = es_sentencia ? crear_valor_unidad(NULL) : clonar_valor(v);

        // Intentar asignar el valor, y si no se puede, porque estamos intentando
        // cambiar el valor de una variable inmutable, reportar un error.
        if (!asignar_valor_tabla(evaluador->tabla_simbolos, clonar_string(nombre), v, exp->tipo)) {
            borrar_valor(&retorno);
            Error error = crear_error_reasignando_inmutable(string_a_str(&nombre));
            return crear_valor_error(error, &exp->nombre.nombre_base.loc);
        }
        v = retorno;
    }

    return v;
}

// Una expresión de definición de función, del estilo de \x,y => x+y.
Valor evaluar_expresion_def_funcion(ExpDefFuncion *exp, int es_sentencia, Evaluador *evaluador) {
    // Tenemos que capturar las variables del entorno, copiándolas en una
    // tabla hash, y de paso reportas errores en caso de utilizar variables
    // no definidas en el cuerpo de la función.

    // Lista de identificadores usados en la función que no son ni argumentos
    // ni variables locales (y que por ende, deben provenir del exterior).

    TablaHash *capturadas = malloc(sizeof(TablaHash));
    *capturadas = crear_tabla_hash(exp->variables_capturadas.longitud);

    int funcion_recursiva = 0;
    int i_nombre_funcion_recursiva = -1;

    for(int i = 0; i < exp->variables_capturadas.longitud; ++i) {
        Valor v = recuperar_clon_valor_tabla(*evaluador->tabla_simbolos, exp->variables_capturadas.valores[i]);
        if (v.tipo_valor == TIPO_ERROR) {
            borrar_tabla_hash(capturadas);
            free(capturadas);
            return v;
        }
        // Si es una función intrínseca, no molestarse en capturarla.
        if (v.tipo_valor == TIPO_FUNCION_INTRINSECA) {
            borrar_valor(&v);
            continue;
        }
        // Si la variable es indefinida la única posibilidad es que
        // la variable esté haciendo referencia a la misma función que
        // estamos definiendo (a.k.a, función recursiva).
        // Hay que tratarlo de forma especial para evitar referencias
        // cíclicas.
        if (v.tipo_valor == TIPO_INDEFINIDO) {
            borrar_valor(&v);
            i_nombre_funcion_recursiva = i;
            funcion_recursiva = 1;
            continue;
        }
        insertar_hash(capturadas, clonar_string(exp->variables_capturadas.valores[i].nombre), v, 1);
    }

    Expresion *cuerpo = malloc(sizeof(Expresion));
    *cuerpo = clonar_expresion(*(Expresion*) exp->cuerpo);

    Valor funcion = crear_funcion(clonar_lista_identificadores(exp->nombres_args), (struct Expresion*) cuerpo, (struct TablaHash*) capturadas, exp->loc);

    // Si la función es recursiva, entonces insertar un clon débil de la propia
    // función como variable capturada. Esta no cuenta para el número de referencias
    // activas al valor, por lo que no evitará que se libere su propia memoria.
    if (funcion_recursiva) {
        String nombre_funcion = exp->variables_capturadas.valores[i_nombre_funcion_recursiva].nombre;
        insertar_hash(capturadas, clonar_string(nombre_funcion), clonar_valor_debil(funcion), 1);
    }

    if (es_sentencia) {
        borrar_valor(&funcion);
        return crear_valor_unidad(NULL);
    } else {
        return funcion;
    }
}

Valor evaluar_expresion_bloque(ExpBloque *exp, int es_sentencia, Evaluador *evaluador) {
    // Expresión de una lista de expresiones, en un nivel
    // superior de la tabla de símbolos.

    aumentar_nivel_tabla_simbolos(evaluador->tabla_simbolos);
    ListaExpresiones lista = exp->lista;

    Valor ultimo_valor = crear_valor_unidad(NULL);
    for (int i = 0; i < lista.longitud; ++i) {
        borrar_valor(&ultimo_valor);
        ultimo_valor = evaluar_expresion(evaluador, &((Expresion*) lista.valores)[i]);

        // Si nos encontramos un error o un valor de control de flujo,
        // terminar prematuramente sin evaluar el resto de expresiones.
        if (ultimo_valor.tipo_valor == TIPO_ERROR || ultimo_valor.tipo_valor == TIPO_CONTROL_FLUJO) {
            reducir_nivel_tabla_simbolos(evaluador->tabla_simbolos);
            return ultimo_valor;
        }
    }
    reducir_nivel_tabla_simbolos(evaluador->tabla_simbolos);

    if (es_sentencia) {
        borrar_valor(&ultimo_valor);
        return crear_valor_unidad(NULL);
    }
    return ultimo_valor;
}

Valor evaluar_expresion_import(ExpImporte *exp, Evaluador *evaluador) {
    // Expresión de import, ya sea de un archivo de código del lenguaje,
    // que tendremos que evaluar con un Lexer y un Evaluador; o de un
    // archivo biblioteca dinámica de C.

    String dir_archivo = clonar_string(evaluador->wd);
    extender_string(&dir_archivo, "/");
    extender_string(&dir_archivo, string_a_str(&exp->archivo));

    if (exp->foraneo) {
        // Importar una biblioteca dinámica de C

        BibilotecaDinamica bib = cargar_biblioteca_dinamica(string_a_str(&dir_archivo));
        if (!bib) {
            Error error = crear_error("No se pudo abrir la biblioteca dinámica \"%s\".",
                                      string_a_str(&dir_archivo));
            borrar_string(&dir_archivo);
            return crear_valor_error(error, exp->loc);
        }

        Valor v = crear_valor_biblioteca(bib, &exp->as->loc);
        if (!asignar_valor_tabla(evaluador->tabla_simbolos, exp->as->nombre, v, ASIGNACION_INMUTABLE)) {
            Error error = crear_error("Ya hay un objeto definido con este nombre");
            return crear_valor_error(error, &exp->as->loc);
        }
    } else {
        // Importar un archivo normal con un lexer.

        CodigoFuente src;
        if (!crear_codigo_fuente_archivo(string_a_str(&dir_archivo), &src)) {
            Error error = crear_error("No se pudo abrir el archivo \"%s\"", string_a_str(&dir_archivo));
            borrar_string(&dir_archivo);
            return crear_valor_error(error, exp->loc);
        }

        // Calcular el nuevo directorio de trabajo extrayendo lo
        // que viene antes del último '/' del directorio del import.
        String wd_import = crear_string_vacio();
        char* subdir = string_a_str(&dir_archivo);
        long longitud_subdir = strlen(subdir);
        for (long i = longitud_subdir; i > 0; --i) {
            if (subdir[i] == '/') {
                extender_string_n(&wd_import, subdir, i);
                break;
            }
        }

        // Crear un evaluador en contexto de módulo y con el
        // nuevo directorio de trabajo que hemos calculado.
        Lexer lexer = crear_lexer(src);
        Parser parser = crear_parser(lexer, CONTEXTO_MODULO);

        Evaluador evaluador_import = crear_evaluador(evaluador->tabla_simbolos, wd_import);

        aumentar_nivel_tabla_simbolos(evaluador_import.tabla_simbolos);
        establecer_barrera_tabla_simbolos(evaluador_import.tabla_simbolos);
        Expresion e;
        while (siguiente_expresion(&parser, &e)) {
            Valor v = evaluar_expresion(&evaluador_import, &e);
            if (v.tipo_valor == TIPO_ERROR) imprimir_valor(v);
            borrar_valor(&v);
            borrar_expresion(&e);
        }
        reducir_nivel_tabla_simbolos(evaluador_import.tabla_simbolos);
        borrar_evaluador(&evaluador_import);
        borrar_parser(&parser);
    }

    borrar_string(&dir_archivo);
    return crear_valor_unidad(NULL);
}

Valor evaluar_expresion_condicional(ExpCondicional *exp, int es_sentencia, Evaluador *evaluador) {
    // Expresión condicional del estilo `if cond then a else b`.
    // Evaluar la condición primero; comprobar que es un booleano,
    // y entonces evaluar una de las dos expresiones.

    Valor cond = evaluar_expresion(evaluador, (Expresion*)exp->condicion);
    if (cond.tipo_valor == TIPO_ERROR) return cond;
    if (cond.tipo_valor != TIPO_BOOL) {
        Error error = crear_error("Sólo se pueden utilizar booleanos como condicionales.");
        Valor v = crear_valor_error(error, cond.loc);
        borrar_valor(&cond);
        return v;
    }

    int c = cond.bool;
    borrar_valor(&cond);

    if (c) {
        // La condición era verdadera; evaluar la expresión del "then".
        return evaluar_expresion(evaluador, (Expresion*)exp->verdadero);
    } else if (exp->falso) {
        // La condición era falsa; evaluar la expresión del "else".
        return evaluar_expresion(evaluador, (Expresion*)exp->falso);
    } else {
        // La condición era falsa, pero no había un "else".
        return crear_valor_unidad(NULL);
    }
}

Valor evaluar_expresion_while(ExpWhile *exp, int es_sentencia, Evaluador *evaluador) {
    // Un bucle while del estilo de `while x do y`.
    // Tenemos que evaluar reiteradamente la condición (recordando clonarla,
    // y después evaluar el cuerpo repetidamente).

    do {
        Valor cond = evaluar_expresion(evaluador, (Expresion*) exp->condicion);

        if (cond.tipo_valor == TIPO_ERROR) return cond;
        if (cond.tipo_valor != TIPO_BOOL) {
            Error error = crear_error("Sólo se pueden utilizar booleanos como condicionales.");
            Valor v = crear_valor_error(error, cond.loc);
            borrar_valor(&cond);
            return v;
        }

        int c = cond.bool;
        borrar_valor(&cond);

        if (!c) {
            // La condición era falsa. Terminar.
            return crear_valor_unidad(NULL);
        } else {
            Valor v = evaluar_expresion(evaluador, (Expresion*) exp->cuerpo);

            // Si el cuerpo del bucle dio un error, terminar ya.
            if (v.tipo_valor == TIPO_ERROR) return v;

            // Si el cuerpo devolvió un control de flujo, comprobar
            // si era un exit (en cuyo caso, tenemos que propagar
            // el valor), y si no, capturar el valor de control de flujo.
            if (v.tipo_valor == TIPO_CONTROL_FLUJO) {
                switch (v.control_flujo.tipo) {
                    case CTR_FLUJO_EXIT: return v;
                    default:
                        if (v.control_flujo.valor) {
                            Valor r = *(Valor*) v.control_flujo.valor;
                            free(v.control_flujo.valor);
                            v.control_flujo.valor = NULL;
                            borrar_valor(&v);
                            return r;
                        } else {
                            Valor r = crear_valor_unidad(v.loc);
                            borrar_valor(&v);
                            return r;
                        }
                }
            } else {
                borrar_valor(&v);
            }
        }
    } while (1);
}

Valor evaluar_expresion_control_flujo(ExpControlFlujo *exp, Evaluador *evaluador) {
    // En una expresión de control de flujo siempre se crea un valor de control de flujo,
    // aunque la expresión sea una sentencia.

    if (exp->retorno) {
        // Si hay un valor de retorno asociado al control de flujo (ej, `return value;`),
        // entonces devolverlo en el valor.

        Valor v = evaluar_expresion(evaluador, (Expresion*) exp->retorno);
        return crear_valor_control_flujo(exp->tipo, &v, exp->loc);
    } else {
        // Si no, simplemente crear un valor de control de flujo y terminar.
        return crear_valor_control_flujo(exp->tipo, NULL, exp->loc);
    }
}

Valor evaluar_expresion(Evaluador *evaluador, Expresion *exp) {
    // Imprimir la expresión si estamos en modo debugging.
    if (evaluador->debug) {
        imprimir_expresion(*exp);
    }

    // Actuar de distinta manera en base al tipo de expresión.
    switch (exp->tipo) {
        case EXP_VALOR: return evaluar_expresion_valor(&exp->valor, exp->es_sentencia);
        case EXP_NOMBRE: return evaluar_expresion_nombre(&exp->nombre, exp->es_sentencia, evaluador);
        case EXP_OP_LLAMADA: return evaluar_expresion_llamada(&exp->llamada_funcion, exp->es_sentencia, evaluador);
        case EXP_OP_ASIGNACION: return evaluar_expresion_asignacion(&exp->asignacion, exp->es_sentencia, evaluador);
        case EXP_OP_DEF_FUNCION: return evaluar_expresion_def_funcion(&exp->def_funcion, exp->es_sentencia, evaluador);
        case EXP_BLOQUE: return evaluar_expresion_bloque(&exp->bloque, exp->es_sentencia, evaluador);
        case EXP_IMPORT: return evaluar_expresion_import(&exp->importe, evaluador);
        case EXP_CONDICIONAL: return evaluar_expresion_condicional(&exp->condicional, exp->es_sentencia, evaluador);
        case EXP_BUCLE_WHILE: return evaluar_expresion_while(&exp->bucle_while, exp->es_sentencia, evaluador);
        case EXP_CONTROL_FLUJO: return evaluar_expresion_control_flujo(&exp->control_flujo, evaluador);
        default: return crear_valor_error(crear_error("Expresión desconocida. What?"), NULL);
    }

}

/**
 * Evalúa una lista de expresiones, liberando su memoria, y devolviendo
 * sus resultados en una lista de valores.
 * @param tabla tabla de símbolos a utilizar para la resolución de variables.
 * @param listaExpresiones expresiones a evaluar y que será liberada.
 * @param contexto el contexto en el que se deberían evaluar las expresiones.
 * @param wd el working directory en el que se deberían evaluar las expresiones.
 * @return lista de valores de las expresiones.
 */
ListaValores evaluar_expresiones(Evaluador *evaluador, ListaExpresiones *listaExpresiones) {
    ListaValores valores = crear_lista_valores();

    for (int i = 0; i < listaExpresiones->longitud; ++i) {
        Valor v = evaluar_expresion(evaluador, &((Expresion*)listaExpresiones->valores)[i]);
        push_lista_valores(&valores, v);
    }

    //if (valores.loc && listaExpresiones->loc)
    //    *valores.loc = *listaExpresiones->loc;

    return valores;
}
