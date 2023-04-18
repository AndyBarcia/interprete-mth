#include "evaluador.h"
#include "analizador_sintactico.h"
#include "analizador_lexico.h"
#include "std.h"

Evaluador crear_evaluador(Lexer lexer, ContextoEvaluacion contexto, String wd) {
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

Valor evaluar_expresion(TablaSimbolos *tabla, Expresion *exp, String wd);
ListaValores evaluar_expresiones(TablaSimbolos *tabla, ListaExpresiones *listaExpresiones, String wd);

int evaluar_siguiente(Evaluador *evaluador, TablaSimbolos *tabla_simbolos, Valor *valor) {
    ContextoExpresion ctx = {
        .es_modulo = evaluador->contexto == CONTEXTO_MODULO,
        .es_bucle = 0,
        .es_asignacion = 0,
        .es_funcion = 0,
        .es_bloque = 0
    };

    int status;
    do {
        YYSTYPE token;
        int c = siguiente_componente_lexico(&evaluador->lexer, &token);

        Expresion exp = crear_exp_nula();
        status = yypush_parse((yypstate*) evaluador->ps, c, &token, (YYLTYPE*) &evaluador->lexer.loc, &exp);

        if (exp.tipo != EXP_NULA) {
            validar_expresion(&exp, ctx);
            *valor = evaluar_expresion(tabla_simbolos, &exp, evaluador->wd);
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
Valor evaluar_expresion(TablaSimbolos *tabla, Expresion *exp, String wd) {
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
        case EXP_NOMBRE: {
            // La expresión representa el nombre de un lugar, como una variable
            // `x` o un índice `x.y` o `x[y]`.

            // Primero tenemos que encontrar el nombre base en la tabla de símbolos,
            // reportando en el caso de que no exista la variable.
            Valor valor;
            if ((valor = recuperar_clon_valor_tabla(*tabla, exp->nombre.nombre_base)).tipo_valor == TIPO_ERROR) {
                borrar_nombre_asignable(&exp->nombre);
                return valor;
            }

            // Y luego buscar recursivamente los accesos.
            for (int i = 0; i < exp->nombre.longitud; ++i) {
                Acceso acceso = exp->nombre.accesos[i];

                // Primero evaluamos el índice que vamos a utilizar
                Valor indice;
                switch (acceso.tipo) {
                    case ACCESO_MIEMBRO:
                        indice = crear_valor_string(acceso.miembro.nombre, &acceso.miembro.loc);
                        borrar_loc(&acceso.miembro.loc);
                        break;
                    case ACCESO_INDEXADO:
                        indice = evaluar_expresion(tabla, (Expresion*) acceso.indice, wd);
                        free(acceso.indice);
                        if (indice.tipo_valor == TIPO_ERROR) {
                            borrar_valor(&valor);
                            borrar_identificador(&exp->nombre.nombre_base);
                            for (int j = i+1; j < exp->nombre.longitud; ++j)
                                borrar_acceso(&exp->nombre.accesos[j]);
                            free(exp->nombre.accesos);
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
                    Valor r = crear_valor_error(error, indice.loc);
                    borrar_valor(&valor);
                    borrar_valor(&indice);
                    borrar_identificador(&exp->nombre.nombre_base);
                    for (int j = i+1; j < exp->nombre.longitud; ++j)
                        borrar_acceso(&exp->nombre.accesos[j]);
                    free(exp->nombre.accesos);
                    return r;
                }

                borrar_valor(&valor);
                borrar_valor(&indice);
                valor = miembro;
            }

            // Si la expresión es una sentencia, simplemente liberar la memoria
            // y terminar, porque no tenemos que devolver el valor.
            if (exp->es_sentencia) {
                borrar_valor(&valor);
                borrar_identificador(&exp->nombre.nombre_base);
                if (exp->nombre.accesos) free(exp->nombre.accesos);
                return crear_valor_unidad(NULL);
            }

            // Si no, reemplazar la localización original del valor por la localización
            // del nombre asignable, y devolver el valor obtenido.
            if (valor.loc)
                borrar_loc(valor.loc);
            else
                valor.loc = malloc(sizeof(Localizacion));
            *valor.loc = clonar_loc(exp->nombre.nombre_base.loc);
            borrar_identificador(&exp->nombre.nombre_base);
            if (exp->nombre.accesos) free(exp->nombre.accesos);

            return valor;
        }
        case EXP_OP_LLAMADA: {
            // Si es una llamada, evaluar la expresión que queremos llamar como función,
            // y comprobar que efectivamente es una función.
            Valor f = evaluar_expresion(tabla, (Expresion*)exp->llamada_funcion.funcion, wd);
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
            ListaValores args = evaluar_expresiones(tabla, &exp->llamada_funcion.args, wd);
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

                    // Introducir los argumentos en la tabla de símbolos, introduciendo
                    // una barrera para que las modificaciones de variables dentro de la
                    // función no se propaguen fuera del cuerpo de la función.
                    aumentar_nivel_tabla_simbolos(tabla);
                    establecer_barrera_tabla_simbolos(tabla);

                    for (int i = 0; i < args.longitud; ++i) {
                        String nombre = clonar_string(fn.nombres_args.valores[i].nombre);

                        insertar_hash(&tabla->tablas[tabla->nivel], nombre, args.valores[i], 0);
                        //asignar_valor_tabla(tabla, nombre, args.valores[i], ASIGNACION_NORMAL);
                    }
                    // Introducir las variables capturadas por la función
                    asignar_clones_valores_tabla(tabla, *(TablaHash *) fn.variables_capturadas);

                    // Evaluar la propia función, recordando clonar el cuerpo.
                    Expresion cuerpo = clonar_expresion(*(Expresion*)fn.cuerpo);
                    Valor v = evaluar_expresion(tabla, &cuerpo, wd);

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
            String nombre = exp->asignacion.nombre.nombre_base.nombre;

            // Comportamiento especial cuando estamos asignando una definición
            // de función a una variable. Esto permite que se pueda utilizar
            // recursividad del estilo de `f = \x => f(x-1)`, ya que de forma
            // normal esto causaría un error "variable f no definida" en el
            // cuerpo de la función.
            if (((Expresion*)exp->asignacion.expresion)->tipo == EXP_OP_DEF_FUNCION) {
                //printf("%s\n", string_a_str(&nombre));
                // Asignar un valor especial indefinido.
                TipoAsignacion t = exp->asignacion.tipo != ASIGNACION_EXPORT ? ASIGNACION_NORMAL : ASIGNACION_EXPORT;
                asignar_valor_tabla(tabla, clonar_string(nombre), crear_valor_indefinido(), t);
            }

            // Evaluar el valor que se va a asignar.
            Valor v = evaluar_expresion(tabla, (Expresion*)exp->asignacion.expresion, wd);
            free(exp->asignacion.expresion);

            if (v.tipo_valor != TIPO_ERROR) {
                // Si la asignación es una sentencia, simplemente devolvemos valor unidad.
                // Si no, devolvemos un clon de valor que vamos a insertar en la tabla.
                Valor retorno = exp->es_sentencia ? crear_valor_unidad(NULL) : clonar_valor(v);

                // Intentar asignar el valor, y si no se puede, porque estamos intentando
                // cambiar el valor de una variable inmutable, reportar un error.
                if (!asignar_valor_tabla(tabla, nombre, v, exp->asignacion.tipo)) {
                    Error error = crear_error_reasignando_inmutable(string_a_str(&nombre));
                    v = crear_valor_error(error, &exp->asignacion.nombre.nombre_base.loc);
                    borrar_valor(&retorno);
                    borrar_nombre_asignable(&exp->asignacion.nombre);
                    if(exp->asignacion.loc) {
                        borrar_loc(exp->asignacion.loc);
                        free(exp->asignacion.loc);
                    }
                    return v;
                }
                v = retorno;
            } else {
                // Si el valor era un error no insertamos nada.
                borrar_string(&exp->asignacion.nombre.nombre_base.nombre);
            }

            // Liberar toda la memoria que queda.
            if (exp->asignacion.loc) {
                borrar_loc(exp->asignacion.loc);
                free(exp->asignacion.loc);
            }
            borrar_loc(&exp->asignacion.nombre.nombre_base.loc);
            if (exp->asignacion.nombre.accesos) {
                for(int i = 0; i < exp->asignacion.nombre.longitud; ++i)
                    borrar_acceso(&exp->asignacion.nombre.accesos[i]);
                free(exp->asignacion.nombre.accesos);
            }
            return v;
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
                Valor v;
                if ((v = recuperar_clon_valor_tabla(*tabla, ids.valores[i])).tipo_valor == TIPO_ERROR) {
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
                insertar_hash(capturadas, clonar_string(ids.valores[i].nombre), v, 1);
            }

            Valor funcion = crear_funcion(exp->def_funcion.nombres_args, (struct Expresion*) cuerpo, (struct TablaHash*) capturadas, exp->def_funcion.loc);

            // Si la función es recursiva, entonces insertar un clon débil de la propia
            // función como variable capturada. Esta no cuenta para el número de referencias
            // activas al valor, por lo que no evitará que se libere su propia memoria.
            if (funcion_recursiva) {
                //printf("Función recursiva!");
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
                ultimo_valor = evaluar_expresion(tabla, &((Expresion*) lista.valores)[i], wd);

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

            String dir_archivo = clonar_string(wd);
            extender_string(&dir_archivo, "/");
            extender_string(&dir_archivo, string_a_str(&exp->importe.archivo));

            if (exp->importe.foraneo) {
                // Importar una biblioteca dinámica de C

                BibilotecaDinamica bib = cargar_biblioteca_dinamica(string_a_str(&dir_archivo));
                if (!bib) {
                    Error error = crear_error("No se pudo abrir la biblioteca dinámica \"%s\".",
                                              string_a_str(&dir_archivo));
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
                if (!crear_codigo_fuente_archivo(string_a_str(&dir_archivo), &src)) {
                    Error error = crear_error("No se pudo abrir el archivo \"%s\"", string_a_str(&dir_archivo));
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
                Evaluador evaluador = crear_evaluador(lexer, CONTEXTO_MODULO, wd_import);

                aumentar_nivel_tabla_simbolos(tabla);
                establecer_barrera_tabla_simbolos(tabla);
                Valor x;
                while(evaluar_siguiente(&evaluador, tabla, &x)) {
                    if (x.tipo_valor == TIPO_ERROR) imprimir_valor(x);
                    borrar_valor(&x);
                }
                reducir_nivel_tabla_simbolos(tabla);
                borrar_evaluador(&evaluador);
            }

            borrar_string(&exp->importe.archivo);
            borrar_string(&dir_archivo);
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
            // Evaluar la condición primero; comprobar que es un booleano,
            // y entonces evaluar una de las dos expresiones.

            Valor cond = evaluar_expresion(tabla, (Expresion*)exp->condicional.condicion, wd);
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
                // La condición era verdadera; evaluar la expresión del "then".

                Valor verdadero = evaluar_expresion(tabla, (Expresion*)exp->condicional.verdadero, wd);
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
                // La condición era falsa; evaluar la expresión del "else".

                Valor falso = evaluar_expresion(tabla, (Expresion*)exp->condicional.falso, wd);
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
                // La condición era falsa, pero no había un "else".
                // Simplemente borrar la memoria y devolver unidad.

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
        case EXP_BUCLE_WHILE: {
            // Un bucle while del estilo de `while x do y`.
            // Tenemos que evaluar reiteradamente la condición (recordando clonarla,
            // y después evaluar el cuerpo repetidamente).

            do {
                Expresion cond_exp = clonar_expresion(*(Expresion*) exp->bucle_while.condicion);
                Valor cond = evaluar_expresion(tabla, &cond_exp, wd);
                if (cond.tipo_valor != TIPO_BOOL) {
                    Error error = crear_error("Sólo se pueden utilizar booleanos como condicionales.");
                    Valor v = crear_valor_error(error, cond.loc);
                    borrar_valor(&cond);
                    borrar_expresion(exp);
                    return v;
                }

                if (!cond.bool) {
                    borrar_valor(&cond);
                    borrar_expresion(exp);
                    return crear_valor_unidad(NULL);
                } else {
                    borrar_valor(&cond);
                    Expresion cuerpo_exp = clonar_expresion(*(Expresion*) exp->bucle_while.cuerpo);
                    Valor v = evaluar_expresion(tabla, &cuerpo_exp, wd);

                    // Si el cuerpo del bucle dio un error, terminar ya.
                    if (v.tipo_valor == TIPO_ERROR) {
                        borrar_expresion(exp);
                        return v;
                    }
                    // Si el cuerpo devolvió un control de flujo, comprobar
                    // si era un exit (en cuyo caso, tenemos que propagar
                    // el valor), y si no, capturar el valor de control de flujo.
                    if (v.tipo_valor == TIPO_CONTROL_FLUJO) {
                        borrar_expresion(exp);
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
        case EXP_CONTROL_FLUJO: {
            // En una expresión de control de flujo siempre se crea un valor de control de flujo,
            // aunque la expresión sea una sentencia.

            if (exp->control_flujo.retorno) {
                // Si hay un valor de retorno asociado al control de flujo (ej, `return value;`), entonces
                // devolverlo en el valor.

                Valor v = evaluar_expresion(tabla, (Expresion*) exp->control_flujo.retorno, wd);
                free(exp->control_flujo.retorno);
                Valor r = crear_valor_control_flujo(exp->control_flujo.tipo, &v, exp->control_flujo.loc);
                if (exp->control_flujo.loc) {
                    borrar_loc(exp->control_flujo.loc);
                    free(exp->control_flujo.loc);
                }
                return r;
            } else {
                // Si no, simplemente crear un valor de control de flujo y terminar.

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
ListaValores evaluar_expresiones(TablaSimbolos *tabla, ListaExpresiones *listaExpresiones, String wd) {
    ListaValores valores = crear_lista_valores();

    for (int i = 0; i < listaExpresiones->longitud; ++i) {
        Valor v = evaluar_expresion(tabla, &((Expresion*)listaExpresiones->valores)[i], wd);
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
