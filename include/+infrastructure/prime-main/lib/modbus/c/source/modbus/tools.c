#include <modbus/tools.h>

uint16_t mbs_read_register(const mbs_byte_t* data) {
    uint16_t value;
    mbs_byte_t* lo = (mbs_byte_t*)(&value);
    mbs_byte_t* hi = lo + 1;
    *hi = data[0];
    *lo = data[1];
    return value;
}

void mbs_write_register(mbs_byte_t* data, uint16_t value) {
    mbs_byte_t* lo = (mbs_byte_t*)(&value);
    mbs_byte_t* hi = lo + 1;
    data[0] = *hi;
    data[1] = *lo;
}
