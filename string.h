#ifndef LEXER_STRING_H
#define LEXER_STRING_H

typedef int PunteroString;

#define SMALL_STRING_PADDING 4

/// Array de caracteres que utiliza
/// un array dinámico para cadenas
/// largas (más de 8 caracteres), y
/// el stack para cadenas pequeñas.
///
/// Tiene una serie de funciones
/// `extender_string` y `extender_string_n`
/// para facilitar la creación de
/// strings.
typedef struct {
    /// Contiene el string si el puntero
    /// es NULL.
    char padding[SMALL_STRING_PADDING];
    /// Puntero a memoria dinámica, o
    /// NULL si se está utilizando el
    /// padding.
    PunteroString puntero;
} String;

/**
 * Inicializa el buffer de strings.
 */
void crear_buffer_strings();

/**
 * Borra toda la memoria del buffer.
 */
void borrar_buffer_strings();

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
 * @param string string que se va a extender
 * @param str cadena de caracteres
 */
void extender_string(String *string, char *str);

/**
 * Extiende los contenidos de un string, añadiendo una cierta
 * cantidad de caracteres de una cadena.
 * @param str cadena de caracteres
 * @param n número de caracteres a copiar
 */
void extender_string_n(String *string, char *str, unsigned long n);

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
char *string_a_puntero(String *string);

/**
 * Devuelve la longitud de un string.
 * @param string
 * @return
 */
unsigned long longitud_string(String *string);

#endif //LEXER_STRING_H
