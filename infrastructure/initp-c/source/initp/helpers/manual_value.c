#include <initp/helpers/manual_value.h>

void tools_manual_init_uint8(tools_manual_uint8_t* object, uint8_t value) {
    object->value = value;
    object->manual = 0xFF;
}

uint8_t tools_manual_get_uint8(const tools_manual_uint8_t* object) {
    if (object->manual != 0xFF) {
        return object->manual;
    }
    return object->value;
}
