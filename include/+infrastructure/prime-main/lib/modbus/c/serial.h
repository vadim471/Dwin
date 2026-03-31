#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

#include <modbus/config.h>

#include <initp/platform/serial_port.h>

mbs_size_t mbs_read_serial(void* object, mbs_byte_t* data, mbs_size_t size) {
    return (mbs_size_t)sys_read_serial_port((sys_serial_port_tp)object, (char*)data, (size_t)size);
}

mbs_size_t mbs_write_serial(void* object, const mbs_byte_t* data, mbs_size_t length) {
    return (mbs_size_t)sys_write_serial_port((sys_serial_port_tp)object, (const char*)data, (size_t)length);
}

#endif // SERIAL_H_INCLUDED
