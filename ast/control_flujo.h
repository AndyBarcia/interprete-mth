#ifndef PRACTICA3_CONTROL_FLUJO_H
#define PRACTICA3_CONTROL_FLUJO_H

#include "../analizador_lexico.h"

/// Los tipos de control de flujo del código.
typedef enum {
    /// Terminar prematuramente la ejecución
    /// de una función, devolviendo el valor
    /// de retorno asociado.
    CTR_FLUJO_RETURN,
    /// Termina prematuramente la ejecución
    /// de un bucle, devolviendo el valor
    /// asociado.
    CTR_FLUJO_BREAK,
    /// Termina la ejecución del programa
    /// prematuramente, devolviendo el valor
    /// de retorno como código de salida.
    CTR_FLUJO_EXIT
} TipoControlFlujo;

char* tipo_control_flujo_a_str(TipoControlFlujo tipo);

#endif //PRACTICA3_CONTROL_FLUJO_H
