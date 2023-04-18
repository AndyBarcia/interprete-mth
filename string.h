#ifndef LEXER_STRING_H
#define LEXER_STRING_H

#define SMALL_STRING_PADDING sizeof(int*)

/// Array de caracteres con las siguientes características:
///  * Se utiliza un array dinámico para cadenas largas
///    (más de 8 caracteres, o mayor que el  tamaño de un
///    puntero), y el stack para cadenas pequeñas.
///  * Con arrays dinámicos se utiliza una cuenta de
///    referencias dinámicas para un clonado barato de strings.
///  * Cuenta con una serie de funciones `extender_string`
///    y `extender_string_n` para facilitar la creación de
///    strings.
typedef struct {
    union {
        /// Contiene el string si el puntero
        /// es NULL.
        char padding[SMALL_STRING_PADDING];
        /// El número de referencias dinámicas
        /// a este string. Su memoria no se
        /// liberará hasta que desaparezcan
        /// todas las referencias.
        /// Sólo se utiliza si no se está
        /// utilizando el padding.
        int* referencias;
    };
    /// Puntero a memoria dinámica, o
    /// NULL si se está utilizando el
    /// padding.
    char* puntero;
} String;

/**
 * Crea un nuevo string vacío.
 * No utiliza memoria dinámica.
 * @return el nuevo string.
 */
String crear_string_vacio();

/**
 * Crea un nuevo string copiando los contenidos
 * de una cadena de caracteres delimitada por '\0'.
 * @param str cadena de caracteres.
 * @return nueva cadena.
 */
String crear_string(char *str);

/**
 * Crea un nuevo string escapando los caracteres
 * especiales de una cadena dada.
 * @param str la cadena que se va a escapar.
 * @return el nuevo string.
 */
String crear_string_escapado(char *str);

/**
 * Crea un nuevo string con una capacidad dada
 * (sin contar el '\0' final), y con unos contenidos
 * sin especificar.
 * @param n longitud reservada
 * @return
 */
String crear_string_n(unsigned long n);

/**
 * Extiende los contenidos de un string, añadiendo los
 * contenidos de una cadena de caracteres al final del string.
 * Si el string es una referencia única, se extiende el propio
 * string; pero si no, se hace un clonado profundo para evitar
 * problemas de aliasing.
 * @param string string que se va a extender
 * @param str cadena de caracteres
 */
void extender_string(String *string, char *str);

/**
 * Extiende los contenidos de un string, añadiendo una cierta
 * cantidad de caracteres de una cadena.
 * Si el string es una referencia única, se extiende el propio
 * string; pero si no, se hace un clonado profundo para evitar
 * problemas de aliasing.
 * @param string el string a extender.
 * @param str cadena de caracteres
 * @param n número de caracteres a copiar
 */
void extender_string_n(String *string, char *str, unsigned long n);

/**
 * Realiza un clonado de un string, aumentando su número de
 * referencias dinámicas en caso de cadenas largas.
 * Es muy barato de llamar.
 * @param string
 * @return
 */
String clonar_string(String string);

/**
 * Libera los contenidos del string.
 * Si no se utiliza memoria dinámica, no se hace nada
 * @param string
 */
void borrar_string(String *string);

/**
 * Convierte un string a un puntero a un array de
 * caracteres normal y corriente.
 * @param string
 * @return
 */
char *string_a_str(String *string);

/**
 * Devuelve la longitud de un string.
 * @param string
 * @return
 */
unsigned long longitud_string(String *string);

#endif //LEXER_STRING_H
