#include "evaluador.h"
#include "analizador_sintactico.h"
#include "analizador_lexico.h"
#include "std.h"

Evaluador crear_evaluador(Lexer lexer, Contexto contexto, String wd) {
    return (Evaluador) {
        .lexer = lexer,
        .ps = yypstate_new(),
        .contexto = contexto,
        .wd = wd
    };
}

void borrar_evaluador(Evaluador *evaluador) {
    yypstate_delete((yypstate*) evaluador->ps);
    borrar_analizador_lexico(&evaluador->lexer);
    borrar_string(&evaluador->wd);
}

Valor evaluar_expresion(TablaSimbolos *tabla, Expresion *exp, Contexto contexto, String wd);
ListaValores evaluar_expresiones(TablaSimbolos *tabla, ListaExpresiones *listaExpresiones, Contexto contexto, String wd);

int evaluar_siguiente(Evaluador *evaluador, TablaSimbolos *tabla_simbolos, Valor *valor) {
    int status;
    do {
        YYSTYPE token;
        int c = siguiente_componente_lexico(&evaluador->lexer, &token);

        Expresion exp = crear_exp_nula();
        status = yypush_parse((yypstate*) evaluador->ps, c, &token, (YYLTYPE*) &evaluador->lexer.loc, &exp);

        if (exp.tipo != EXP_NULA) {
            *valor = evaluar_expresion(tabla_simbolos, &exp, evaluador->contexto, evaluador->wd);
            return 1;
        }
    } while (status == YYPUSH_MORE);
    return 0;
}

/**
 * Evalúa el valor de una expresión, liberando la memoria de la expresión
 * y devolviendo su resultado en un valor.
 * @param tabla tabla de símbolos a utilizar para la resolución de variables.
 * @param exp expresión a evaluar y que será liberada.
 * @param contexto el contexto en el que se debería evaluar la expresión.
 * @param wd el working directory en el que se debería evaluar la expresión.
 * @return valor de la expresión.
 */
Valor evaluar_expresion(TablaSimbolos *tabla, Expresion *exp, Contexto contexto, String wd) {
    // Hay que tener en cuenta que sólo hay que devolver un valor
    // si la expresión no es una sentencia. Si no, se tiene que
    // devolver el valor unidad.
    // Si en algún momento nos encontramos con un valor de tipo
    // "error", hay que abortar y devolver ese valor, aunque la
    // expresión sea una sentencia.
    switch (exp->tipo) {
        case EXP_VALOR: {
            // La expresión es un valor; devolverlo si no es una sentencia.
            if (exp->es_sentencia && exp->valor.tipo_valor != TIPO_ERROR) {
                borrar_valor(&exp->valor);
                return crear_valor_unidad(NULL);
            }
            return exp->valor;
        }
        case EXP_IDENTIFICADOR: {
            // Si la expresión es un identificador; buscar su valor en la tabla de símbolos.
            Valor v = recuperar_valor_tabla(*tabla, exp->nombre);
            if (v.tipo_valor == TIPO_ERROR) {
                borrar_identificador(&exp->nombre);
                return v;
            }
            if (v.tipo_valor == TIPO_INDEFINIDO) {
                Error error = crear_error("Haciendo referencia a un valor aún no definido.");
                Valor r = crear_valor_error(error, &exp->nombre.loc);
                borrar_valor(&v);
                borrar_identificador(&exp->nombre);
                return r;
            }

            if (exp->es_sentencia) {
                borrar_valor(&v);
                return crear_valor_unidad(NULL);
            }

            // Reemplazar la localización original del valor por la localización del identificador.
            if (v.loc)
                borrar_loc(v.loc);
            else
                v.loc = malloc(sizeof(Localizacion));
            *v.loc = exp->nombre.loc;
            borrar_string(&exp->nombre.nombre);
            
            return v;
        }
        case EXP_ACCESO_MIEMBRO: {
            // Si es una expresión de acceso, estilo "objeto.miembro", tenemos que evaluar
            // primero el objeto, y después tratar de acceder a su miembro.

            Valor v = evaluar_expresion(tabla, (Expresion *) exp->acceso.valor, contexto, wd);
            free(exp->acceso.valor);
            if (v.tipo_valor == TIPO_ERROR) return v;

            // Por ahora los únicos objetos de este tipo que se pueden acceder son
            // tests dinámicas de C.
            Valor result = crear_valor_unidad(NULL);
            switch (v.tipo_valor) {
                case TIPO_BIBLIOTECA_FORANEA: {
                    BibilotecaDinamica bib = v.biblioteca;
                    FuncionForanea f = cargar_funcion_biblioteca(bib, identificador_a_str(&exp->acceso.miembro));
                    if (f) {
                        result = crear_funcion_foranea(f);
                    } else {
                        Error error = crear_error("No existe la función foránea \"%s\" en la biblioteca.", identificador_a_str(&exp->acceso.miembro));
                        result = crear_valor_error(error, &exp->acceso.miembro.loc);
                    }
                    break;
                }
                default: {
                    Error error = crear_error("No se puede acceder al miembro \"%s\".", identificador_a_str(&exp->acceso.miembro));
                    result = crear_valor_error(error, &exp->acceso.miembro.loc);
                    break;
                }
            }

            borrar_valor(&v);
            borrar_identificador(&exp->acceso.miembro);
            if (exp->acceso.loc) {
                borrar_loc(exp->acceso.loc);
                free(exp->acceso.loc);
            }
            if (exp->es_sentencia && result.tipo_valor != TIPO_ERROR) {
                borrar_valor(&result);
                return crear_valor_unidad(NULL);
            }
            return result;
        }
        case EXP_OP_LLAMADA: {
            // Si es una llamada, evaluar la expresión que queremos llamar como función,
            // y comprobar que efectivamente es una función.
            Valor f = evaluar_expresion(tabla, (Expresion*)exp->llamada_funcion.funcion, contexto, wd);
            free(exp->llamada_funcion.funcion);
            if (f.tipo_valor == TIPO_ERROR) {
                borrar_lista_expresiones(&exp->llamada_funcion.args);
                if(exp->llamada_funcion.loc) {
                    borrar_loc(exp->llamada_funcion.loc);
                    free(exp->llamada_funcion.loc);
                }
                return f;
            }

            // Obtener la lista de argumentos y abortar si alguno de ellos es un error.
            ListaValores args = evaluar_expresiones(tabla, &exp->llamada_funcion.args, CNTXT_ARGS, wd);
            for(int i = 0; i < args.longitud; ++i) {
                Valor v = ((Valor*)args.valores)[i];
                if (v.tipo_valor == TIPO_ERROR) {
                    // Borrar todos los valores menos el que vamos a devolver.
                    for (int j = 0; j < args.longitud; ++j)
                        if (j != i) borrar_valor(&((Valor*)args.valores)[j]);
                    free(args.valores);
                    if (args.loc) {
                        borrar_loc(args.loc);
                        free(args.loc);
                    }
                    if (exp->llamada_funcion.loc) {
                        borrar_loc(exp->llamada_funcion.loc);
                        free(exp->llamada_funcion.loc);
                    }
                    if (f.loc) {
                        borrar_loc(f.loc);
                        free(f.loc);
                    }
                    return v;
                }
            }

            // Tenemos que actuar de forma distinta en base a si es una
            // función intrínseca o una función definida por el usuario.
            switch (f.tipo_valor) {
                case TIPO_FUNCION_INTRINSECA: {
                    // Una función intrínseca se ejecuta simplemente pasando
                    // el tipo de función junto a los argumentos y la tabla de símbolos.

                    Valor result = ejecutar_funcion_intrinseca(f.funcion_intrinseca, args, tabla, wd);
                    borrar_valor(&f);

                    if (exp->llamada_funcion.loc) {
                        borrar_loc(exp->llamada_funcion.loc);
                        free(exp->llamada_funcion.loc);
                    }
                    if (exp->es_sentencia) {
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
                        Valor v = crear_valor_error(error, exp->llamada_funcion.loc);
                        borrar_lista_valores(&args);
                        if (exp->llamada_funcion.loc) {
                            borrar_loc(exp->llamada_funcion.loc);
                            free(exp->llamada_funcion.loc);
                        }
                        borrar_valor(&f);
                        return v;
                    }

                    // Introducir los argumentos en la tabla de símbolos.
                    aumentar_nivel_tabla_simbolos(tabla);
                    for (int i = 0; i < args.longitud; ++i) {
                        String nombre = clonar_string(fn.nombres_args.valores[i].nombre);
                        asignar_valor_tabla(tabla, nombre, args.valores[i], ASIGNACION_NORMAL);
                    }
                    // Introducir las variables capturadas por la función
                    asignar_clones_valores_tabla(tabla, *(TablaHash *) fn.variables_capturadas);

                    // Evaluar la propia función, recordando clonar el cuerpo.
                    Expresion cuerpo = clonar_expresion(*(Expresion*)fn.cuerpo);
                    Valor v = evaluar_expresion(tabla, &cuerpo, CNTXT_FUNCION, wd);

                    // Si se devolvió un valor de control de flujo, tenemos que
                    // sacar el valor del elemento de control (o propagarlo en el
                    // caso de ser un control de flujo tipo exit).
                    if (v.tipo_valor == TIPO_CONTROL_FLUJO) {
                        switch (v.control_flujo.tipo) {
                            case CTR_FLUJO_EXIT: break;
                            default:
                                if (v.control_flujo.valor) {
                                    Valor r = *(Valor*) v.control_flujo.valor;
                                    free(v.control_flujo.valor);
                                    v.control_flujo.valor = NULL;
                                    borrar_valor(&v);
                                    v = r;
                                } else {
                                    Valor r = crear_valor_unidad(v.loc);
                                    borrar_valor(&v);
                                    v = r;
                                }
                        }
                    }

                    // Una vez terminado reducir nuevamente el nivel de la tabla
                    // y libera la memoria.
                    reducir_nivel_tabla_simbolos(tabla);
                    free(args.valores);
                    if (args.loc) {
                        borrar_loc(args.loc);
                        free(args.loc);
                    }
                    if (exp->llamada_funcion.loc) {
                        borrar_loc(exp->llamada_funcion.loc);
                        free(exp->llamada_funcion.loc);
                    }
                    borrar_valor(&f);

                    if (exp->es_sentencia) {
                        borrar_valor(&v);
                        return crear_valor_unidad(NULL);
                    }
                    return v;
                }
                case TIPO_FUNCION_FORANEA: {
                    Error error = crear_error("No se puede llamar a una función foránea directamente "
                                              "porque su tipo de retorno no es conocido.\nSi quieres "
                                              "llamar a una función foránea, utiliza `callforeign`.");
                    return crear_valor_error(error, f.loc);
                }
                default: {
                    return crear_valor_error(crear_error("Este valor no es una función!"), f.loc);
                }
            }
        }
        case EXP_OP_ASIGNACION: {
            // Expresión de asignación de una expresión a una variable.
            // Tenemos que evaluar la expresión y después comprobar que
            // la podemos insertar en la tabla de símbolos.

            // Sólo se permiten hacer exports en el contexto de un módulo.
            if (exp->asignacion.tipo == ASIGNACION_EXPORT && contexto != CNTXT_MODULO) {
                Error error = crear_error_contexto_incorrecto("export", "fuera de un módulo");
                Valor v = crear_valor_error(error, exp->asignacion.loc);
                borrar_expresion(exp);
                return v;
            }

            if (!asignar_valor_tabla(tabla, exp->asignacion.identificador.nombre, crear_valor_indefinido(), ASIGNACION_NORMAL)) {
                Error error = crear_error(
                        "Intentando reasignar variable inmutable \"%s\"",
                        identificador_a_str(&exp->asignacion.identificador));
                Valor v = crear_valor_error(error, &exp->asignacion.identificador.loc);
                borrar_expresion(exp);
                return v;
            }

            Valor v = evaluar_expresion(tabla, (Expresion*)exp->asignacion.expresion, CNTXT_ASIGNACION, wd);
            free(exp->asignacion.expresion);

            if (v.tipo_valor == TIPO_ERROR) {
                borrar_identificador(&exp->asignacion.identificador);
                if (exp->asignacion.loc) {
                    borrar_loc(exp->asignacion.loc);
                    free(exp->asignacion.loc);
                }
                return v;
            }

            if (exp->asignacion.loc) {
                borrar_loc(exp->asignacion.loc);
                free(exp->asignacion.loc);
            }

            // Si la asignación es una sentencia, simplemente devolvemos valor unidad.
            // Si no, devolvemos un clon de valor que vamos a insertar en la tabla.
            Valor retorno = exp->es_sentencia ? crear_valor_unidad(NULL) : clonar_valor(v);

            borrar_loc(&exp->asignacion.identificador.loc);
            asignar_valor_tabla(tabla, exp->asignacion.identificador.nombre, v, exp->asignacion.tipo);
            return retorno;
        }
        case EXP_OP_DEF_FUNCION: {
            // Una expresión de definición de función, del estilo de \x,y => x+y.
            // Tenemos que capturar las variables del entorno, copiándolas en una
            // tabla hash, y de paso reportas errores en caso de utilizar variables
            // no definidas en el cuerpo de la función.

            Expresion *cuerpo = (Expresion*) exp->def_funcion.cuerpo;

            // Lista de identificadores usados en la función que no son ni argumentos
            // ni variables locales (y que por ende, deben provenir del exterior).
            ListaIdentificadores ids = variables_capturadas(exp->def_funcion);

            TablaHash *capturadas = malloc(sizeof(TablaHash));
            *capturadas = crear_tabla_hash(ids.longitud);

            int funcion_recursiva = 0;
            int i_nombre_funcion_recursiva = -1;

            for(int i = 0; i < ids.longitud; ++i) {
                Valor v = recuperar_valor_tabla(*tabla, ids.valores[i]);
                // Si la variable no pudo ser capturada, propagar el error.
                if (v.tipo_valor == TIPO_ERROR) {
                    borrar_expresion(exp);
                    borrar_lista_identificadores(&ids);
                    borrar_tabla_hash(capturadas);
                    free(capturadas);
                    return v;
                }
                // Si es una función intrínseca, no molestarse en capturarla.
                if (v.tipo_valor == TIPO_FUNCION_INTRINSECA) {
                    borrar_valor(&v);
                    continue;
                }
                // Si la variable es indefinida, y estamos en un contexto de
                // asignación, la única posibilidad es que la variable esté
                // haciendo referencia a la misma función que estamos definiendo
                // (a.k.a, función recursiva).
                // Hay que tratarlo de forma especial para evitar referencias
                // cíclicas.
                if (v.tipo_valor == TIPO_INDEFINIDO && contexto == CNTXT_ASIGNACION) {
                    borrar_valor(&v);
                    i_nombre_funcion_recursiva = i;
                    funcion_recursiva = 1;
                    continue;
                }
                insertar_hash(capturadas, clonar_string(ids.valores[i].nombre), v, 1);
            }

            Valor funcion = crear_funcion(exp->def_funcion.nombres_args, (struct Expresion*) cuerpo, (struct TablaHash*) capturadas, exp->def_funcion.loc);

            // Si la función es recursiva, entonces insertar un clon débil de la propia
            // función como variable capturada. Esta no cuenta para el número de referencias
            // activas al valor, por lo que no evitará que se libere su propia memoria.
            if (funcion_recursiva) {
                Valor clon_funcion = clonar_valor_debil(funcion);
                insertar_hash(capturadas, clonar_string(ids.valores[i_nombre_funcion_recursiva].nombre), clon_funcion, 1);
            }
            borrar_lista_identificadores(&ids);

            if (exp->def_funcion.loc) {
                borrar_loc(exp->def_funcion.loc);
                free(exp->def_funcion.loc);
            }

            if (exp->es_sentencia) {
                borrar_valor(&funcion);
                return crear_valor_unidad(NULL);
            } else {
                return funcion;
            }
        }
        case EXP_BLOQUE: {
            // Expresión de una lista de expresiones, en un nivel
            // superior de la tabla de símbolos.

            aumentar_nivel_tabla_simbolos(tabla);
            ListaExpresiones lista = exp->bloque.lista;
            if(exp->bloque.loc) {
                borrar_loc(exp->bloque.loc);
                free(exp->bloque.loc);
            }

            Valor ultimo_valor = crear_valor_unidad(NULL);
            for (int i = 0; i < lista.longitud; ++i) {
                borrar_valor(&ultimo_valor);
                ultimo_valor = evaluar_expresion(tabla, &((Expresion*) lista.valores)[i], contexto, wd);

                // Si nos encontramos un error o un valor de control de flujo,
                // terminar prematuramente sin evaluar el resto de expresiones.
                if (ultimo_valor.tipo_valor == TIPO_ERROR || ultimo_valor.tipo_valor == TIPO_CONTROL_FLUJO) {
                    for (int j = i+1; j < lista.longitud; ++j)
                        borrar_expresion(&((Expresion*) lista.valores)[j]);
                    free(lista.valores);
                    reducir_nivel_tabla_simbolos(tabla);
                    return ultimo_valor;
                }
            }
            reducir_nivel_tabla_simbolos(tabla);

            free(lista.valores);
            if (exp->es_sentencia) {
                borrar_valor(&ultimo_valor);
                return crear_valor_unidad(NULL);
            }
            return ultimo_valor;
        }
        case EXP_IMPORT: {
            // Expresión de import, ya sea de un archivo de código del lenguaje,
            // que tendremos que evaluar con un Lexer y un Evaluador; o de un
            // archivo biblioteca dinámica de C.

            String dir_archivo = crear_string(string_a_puntero(&wd));
            extender_string(&dir_archivo, "/");
            extender_string(&dir_archivo, string_a_puntero(&exp->importe.archivo));

            if (exp->importe.foraneo) {
                // Importar una biblioteca dinámica de C

                BibilotecaDinamica bib = cargar_biblioteca_dinamica(string_a_puntero(&dir_archivo));
                if (!bib) {
                    Error error = crear_error("No se pudo abrir la biblioteca dinámica \"%s\".", string_a_puntero(&dir_archivo));
                    borrar_string(&exp->importe.archivo);
                    borrar_string(&dir_archivo);
                    if (exp->importe.as) {
                        borrar_identificador(exp->importe.as);
                        free(exp->importe.as);
                    }

                    Valor v = crear_valor_error(error, exp->importe.loc);
                    if (exp->importe.loc) {
                        borrar_loc(exp->importe.loc);
                        free(exp->importe.loc);
                    }
                    return v;
                }

                Valor v = crear_valor_biblioteca(bib, &exp->importe.as->loc);
                if (!asignar_valor_tabla(tabla, exp->importe.as->nombre, v, ASIGNACION_INMUTABLE)) {
                    Error error = crear_error("Ya hay un objeto definido con este nombre");
                    borrar_string(&exp->importe.archivo);
                    if (exp->importe.as) {
                        borrar_identificador(exp->importe.as);
                        free(exp->importe.as);
                    }
                    if (exp->importe.loc) {
                        borrar_loc(exp->importe.loc);
                        free(exp->importe.loc);
                    }
                    return crear_valor_error(error, &exp->importe.as->loc);
                }
            } else {
                // Importar un archivo normal con un lexer.

                CodigoFuente src;
                if (!crear_codigo_fuente_archivo(string_a_puntero(&dir_archivo), &src)) {
                    Error error = crear_error("No se pudo abrir el archivo \"%s\"", string_a_puntero(&dir_archivo));
                    borrar_string(&exp->importe.archivo);
                    borrar_string(&dir_archivo);
                    if (exp->importe.as) {
                        borrar_identificador(exp->importe.as);
                        free(exp->importe.as);
                    }
                    Valor v = crear_valor_error(error, exp->importe.loc);
                    if (exp->importe.loc) {
                        borrar_loc(exp->importe.loc);
                        free(exp->importe.loc);
                    }
                    return v;
                }

                // Calcular el nuevo directorio de trabajo extrayendo
                // lo que viene antes del último '/' del path del import.
                String wd_import = crear_string(string_a_puntero(&wd));
                char* subdir = string_a_puntero(&exp->importe.archivo);
                long longitud_subdir = strlen(subdir);
                for (long i = longitud_subdir; i > 0; --i) {
                    if (subdir[i] == '/') {
                        subdir[i] = '\0';
                        break;
                    }
                }
                if (subdir[0] != '\0') {
                    extender_string(&wd_import, "/");
                    extender_string(&wd_import, subdir);
                }

                // Crear un evaluador en contexto de módulo y con el
                // nuevo directorio de trabajo que hemos calculado.
                Lexer lexer = crear_lexer(src);
                Evaluador evaluador = crear_evaluador(lexer, CNTXT_MODULO, wd_import);

                aumentar_nivel_tabla_simbolos(tabla);

                Valor x;
                while(evaluar_siguiente(&evaluador, tabla, &x)) {
                    if (x.tipo_valor == TIPO_ERROR)
                        imprimir_error(x.error, x.loc);
                    borrar_valor(&x);
                }
                reducir_nivel_tabla_simbolos(tabla);
                borrar_evaluador(&evaluador);
            }

            borrar_string(&exp->importe.archivo);
            if (exp->importe.as) {
                borrar_identificador(exp->importe.as);
                free(exp->importe.as);
            }
            if (exp->importe.loc) {
                borrar_loc(exp->importe.loc);
                free(exp->importe.loc);
            }
            return crear_valor_unidad(NULL);
        }
        case EXP_CONDICIONAL: {
            // Expresión condicional del estilo `if cond then a else b`.

            Valor cond = evaluar_expresion(tabla, (Expresion*)exp->condicional.condicion, contexto, wd);
            free(exp->condicional.condicion);
            if (cond.tipo_valor == TIPO_ERROR) {
                borrar_expresion((Expresion*) exp->condicional.verdadero);
                if (exp->condicional.falso) borrar_expresion((Expresion*) exp->condicional.falso);
                if (exp->condicional.loc) {
                    borrar_loc(exp->condicional.loc);
                    free(exp->condicional.loc);
                }
                return cond;
            }
            if (cond.tipo_valor != TIPO_BOOL) {
                Error error = crear_error("Sólo se pueden utilizar booleanos como condicionales.");
                Valor v = crear_valor_error(error, cond.loc);
                borrar_valor(&cond);
                borrar_expresion((Expresion*) exp->condicional.verdadero);
                if (exp->condicional.falso) borrar_expresion((Expresion*) exp->condicional.falso);
                if (exp->condicional.loc) {
                    borrar_loc(exp->condicional.loc);
                    free(exp->condicional.loc);
                }
                return v;
            }

            if (cond.bool) {
                Valor verdadero = evaluar_expresion(tabla, (Expresion*)exp->condicional.verdadero, contexto, wd);
                free(exp->condicional.verdadero);
                if (exp->condicional.falso) {
                    borrar_expresion((Expresion*) exp->condicional.falso);
                    free(exp->condicional.falso);
                }
                if (exp->condicional.loc) {
                    borrar_loc(exp->condicional.loc);
                    free(exp->condicional.loc);
                }
                borrar_valor(&cond);

                return verdadero;
            } else if (exp->condicional.falso) {
                Valor falso = evaluar_expresion(tabla, (Expresion*)exp->condicional.falso, contexto, wd);
                free(exp->condicional.falso);
                borrar_expresion((Expresion*) exp->condicional.verdadero);
                free(exp->condicional.verdadero);
                if (exp->condicional.loc) {
                    borrar_loc(exp->condicional.loc);
                    free(exp->condicional.loc);
                }
                borrar_valor(&cond);

                return falso;
            } else {
                borrar_expresion((Expresion*) exp->condicional.verdadero);
                free(exp->condicional.verdadero);
                if (exp->condicional.loc) {
                    borrar_loc(exp->condicional.loc);
                    free(exp->condicional.loc);
                }
                borrar_valor(&cond);
                return crear_valor_unidad(NULL);
            }
        }
        case EXP_CONTROL_FLUJO: {
            // En una expresión de control de flujo siempre se crea un valor de control de flujo,
            // aunque la expresión sea una sentencia.

            // Tenemos que comprobar que no estamos utilizando controles de flujo en los contextos
            // incorrectos (por ejemplo, un break fuera de un bucle, o un return fuera de una función).
            if (exp->control_flujo.tipo == CTR_FLUJO_RETURN && contexto != CNTXT_FUNCION) {
                Error error = crear_error_contexto_incorrecto("return", "fuera de una función");
                Valor v = crear_valor_error(error, exp->control_flujo.loc);
                borrar_expresion(exp);
                return v;
            }
            if (exp->control_flujo.tipo == CTR_FLUJO_BREAK && contexto != CNTXT_BUCLE) {
                Error error = crear_error_contexto_incorrecto("break", "fuera de un bucle");
                Valor v = crear_valor_error(error, exp->control_flujo.loc);
                borrar_expresion(exp);
                return v;
            }

            // Si hay un valor de retorno asociado al control de flujo (ej, `return value;`), entonces
            // devolverlo en el valor.
            if (exp->control_flujo.retorno) {
                Valor v = evaluar_expresion(tabla, (Expresion*) exp->control_flujo.retorno, contexto, wd);
                free(exp->control_flujo.retorno);
                Valor r = crear_valor_control_flujo(exp->control_flujo.tipo, &v, exp->control_flujo.loc);
                if (exp->control_flujo.loc) {
                    borrar_loc(exp->control_flujo.loc);
                    free(exp->control_flujo.loc);
                }
                return r;
            } else {
                Valor r = crear_valor_control_flujo(exp->control_flujo.tipo, NULL, exp->control_flujo.loc);
                if (exp->control_flujo.loc) {
                    borrar_loc(exp->control_flujo.loc);
                    free(exp->control_flujo.loc);
                }
                return r;
            }
        }
        default: {
            borrar_expresion(exp);
            return crear_valor_error(crear_error("Expresión desconocida. What?"), NULL);
        }
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
ListaValores evaluar_expresiones(TablaSimbolos *tabla, ListaExpresiones *listaExpresiones, Contexto contexto, String wd) {
    ListaValores valores = crear_lista_valores();

    for (int i = 0; i < listaExpresiones->longitud; ++i) {
        Valor v = evaluar_expresion(tabla, &((Expresion*)listaExpresiones->valores)[i], contexto, wd);
        push_lista_valores(&valores, v);
    }

    //if (valores.loc && listaExpresiones->loc)
    //    *valores.loc = *listaExpresiones->loc;

    free(listaExpresiones->valores);
    if (listaExpresiones->loc) {
        borrar_loc(listaExpresiones->loc);
        free(listaExpresiones->loc);
    }
    listaExpresiones->longitud = 0;
    listaExpresiones->capacidad = 0;
    return valores;
}
