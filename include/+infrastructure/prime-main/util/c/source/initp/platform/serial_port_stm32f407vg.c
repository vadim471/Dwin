#include <initp/platform/serial_port.h>

#include "com_ioapi.h"

#ifdef SYSTEM_TRACE_SERIAL_PORT
#include <stdio.h>
#endif // SYSTEM_TRACE_SERIAL_PORT

void sys_init_serial_port(sys_serial_port_tp object) {
    object->portId = -1;
}

uint8_t sys_open_serial_port(sys_serial_port_tp object, const char* path, uint32_t baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity) {
	int port_index;
	comConfigStruct ccs;

	// try open
	port_index = comGetIndex( path );
	if ( port_index < 0 ) return 0;

	// params conversion
	ccs.baudRate = baudrate;
	switch (databits) {
		case 7:
			ccs.dataBits = cdb_7b;
            break;
		case 8:
			ccs.dataBits = cdb_8b;
            break;
		default:
			return 0;
	}
	switch (stopbits) {
		case 1:
			ccs.stopBits = csb_1;
            break;
		case 2:
			ccs.stopBits = csb_2;
            break;
		default:
			return 0;
	}
	switch (parity) {
		case 0:
			ccs.parity = cpar_No;
            break;
		case 1:
			ccs.parity = cpar_Odd;
            break;
		case 2:
			ccs.parity = cpar_Even;
            break;
		default:
			return 0;
	}
	// set config
	if (comSetConfig(port_index , &ccs)) {
		object->portId = port_index;
		return 1;
	}
	return 0;
}

uint8_t sys_reopen_serial_port(sys_serial_port_tp object) {
    return 1;
}

void sys_close_serial_port(sys_serial_port_tp object) {
	object->portId = -1;
}

size_t sys_read_serial_port(sys_serial_port_tp object, char* data, size_t size) {
	if ( object->portId < 0 ) return 0;
	size_t result = (size_t)comReadData(object->portId, data, size, 0);
	#ifdef SYSTEM_TRACE_SERIAL_PORT
    if (result > 0) {
        printf("Raw read:");
        for (int i = 0; i < result; ++i) {
            printf(" %02x", (unsigned)data[i] & 0xFFU);
        }
        printf(" (%d)\r\n", result);
    }
    #endif // SYSTEM_TRACE_SERIAL_PORT
	return result;
}

size_t sys_write_serial_port(sys_serial_port_tp object, const char* data, size_t length) {
	if ( object->portId < 0 ) return 0;
	size_t result = (size_t)comWriteData(object->portId, (void*)data, length);
	#ifdef SYSTEM_TRACE_SERIAL_PORT
    if (result > 0) {
        printf("Raw write:");
        for (int i = 0; i < result; ++i) {
            printf(" %02x", (unsigned)data[i] & 0xFFU);
        }
        printf(" (%d)\r\n", result);
    }
    #endif // SYSTEM_TRACE_SERIAL_PORT
	return result;
}
