#include "control_flujo.h"

char* tipo_control_flujo_a_str(TipoControlFlujo tipo) {
    switch (tipo) {
        case CTR_FLUJO_RETURN: return "return";
        case CTR_FLUJO_BREAK: return "break";
    }
}