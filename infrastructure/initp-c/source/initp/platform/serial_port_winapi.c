#include <initp/system/debug.h>
#include <initp/tools/format.h>
#include <initp/platform/serial_port.h>

#include <stdio.h>
#include <string.h>
#include <windows.h>

HANDLE winapi_open_serial(const char* port, uint32_t baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity, uint8_t signal) {

    // Make path
    char path[12];
    sprintf(path, "\\\\.\\%s", port);

    // Open serial port
    HANDLE hComm = CreateFileA(path,
                   GENERIC_READ | GENERIC_WRITE,
                   0,
                   NULL,
                   OPEN_EXISTING,
                   0,
                   NULL);
    if (hComm == INVALID_HANDLE_VALUE) {
        if (signal) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "winapi_open_serial", "CreateFile failed with %d", (int)GetLastError());
        }
        return NULL;
    }

    // Flush port
    PurgeComm(hComm, PURGE_RXCLEAR);
    PurgeComm(hComm, PURGE_TXCLEAR);

    // Configure port settings
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    GetCommState(hComm, &dcbSerialParams);
    dcbSerialParams.BaudRate = baudrate;
    dcbSerialParams.ByteSize = databits;
    dcbSerialParams.StopBits = (stopbits == 0) ? ONE5STOPBITS : (stopbits == 2) ? TWOSTOPBITS : ONESTOPBIT;
    dcbSerialParams.Parity   = parity;
    if (!SetCommState(hComm, &dcbSerialParams)) {
        if (signal) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "winapi_open_serial", "SetCommState failed with %d", (int)GetLastError());
        }
        CloseHandle(hComm);
        return NULL;
    }

    // Configure timeouts (in milliseconds)
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout         = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant    = 0;
    timeouts.ReadTotalTimeoutMultiplier  = 0;
    timeouts.WriteTotalTimeoutConstant   = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    if (!SetCommTimeouts(hComm, &timeouts)) {
        if (signal) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "winapi_open_serial", "SetCommTimeouts failed with %d", (int)GetLastError());
        }
        CloseHandle(hComm);
        return NULL;
    }

    // Return handle
    return hComm;
}

void sys_init_serial_port(sys_serial_port_tp object) {
    memset(object, 0x0, sizeof(sys_serial_port_t));
}

uint8_t sys_configure_serial_port(sys_serial_port_tp object, const char* path, uint32_t baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_configure_serial_port", "Serial port pointer is null");
        return 0;
    }
    if (!path) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_configure_serial_port", "Serial port path is null");
        return 0;
    }

    size_t length = strlen(path);
    if (length < 1) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_configure_serial_port", "Serial port path is empty");
        return 0;
    }
    if (length > MAX_SERIAL_PORT_PATH_LENGTH) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_configure_serial_port", "Serial port path too long");
        return 0;
    }

    memcpy(object->path, path, length + 1);
    object->baudrate = baudrate;
    object->databits = databits;
    object->stopbits = stopbits;
    object->parity = parity;
    object->configured = 1;
    return 1;
}

uint8_t sys_open_serial_port(sys_serial_port_tp object, const char* path, uint32_t baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_open_serial_port", "Serial port pointer is null");
        return 0;
    }
    if (!path) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_open_serial_port", "Serial port path is null");
        return 0;
    }

    if (!sys_configure_serial_port(object, path, baudrate, databits, stopbits, parity)) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_open_serial_port", "Failed to configure serial port");
        return 0;
    }

    sys_close_serial_port(object);

    object->opened = 1;
    object->handle = winapi_open_serial(path, baudrate, databits, stopbits, parity, 1);
    if (!object->handle) {
        return 0;
    } else return 1;
}

uint8_t sys_reopen_serial_port(sys_serial_port_tp object) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_reopen_serial_port", "Serial port pointer is null");
        return 0;
    }

    if (object->handle) return 1;
    if (object->opened) return 1;
    if (object->configured) {
        object->opened = 1;
        return 1;
    }
    return 0;
}

void sys_close_serial_port(sys_serial_port_tp object) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_close_serial_port", "Serial port pointer is null");
        return;
    }

    if (object->handle) {
        CloseHandle(object->handle);
        object->handle = NULL;
    }
    object->opened = 0;
}

size_t sys_read_serial_port(sys_serial_port_tp object, char* data, size_t size) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_read_serial_port", "Serial port pointer is null");
        return 0;
    }

    if (!size) return size;

    if (!object->handle) {
        if (object->opened && object->configured) {
            object->handle = winapi_open_serial(object->path, object->baudrate, object->databits, object->stopbits, object->parity, 0);
            if (!object->handle) return 0;
        } else return 0;
    }

    // Temporary character used for reading
    char TempChar;
    DWORD NoBytesRead;
    int i = 0;

    do {
        BOOL result = ReadFile(
            object->handle,
            &TempChar,
            sizeof(TempChar),
            &NoBytesRead,
            NULL
        );

        if (!result) {
            sys_debug_print(SYSTEM_LEVEL_WARNING, "sys_read_serial_port", "ReadFile failed with %d", (int)GetLastError());
            if (object->handle) {
                CloseHandle(object->handle);
                object->handle = NULL;
            }
            return i;
        }

        if (NoBytesRead > 0) {
            // Store Tempchar into buffer
            data[i++] = TempChar;
        } else break;
    } while (i < size);

    #ifdef SYSTEM_TRACE_SERIAL_PORT
    if (i > 0) {
        sys_trace_array("sys_read_serial_port", data, i);
    }
    #endif // SYSTEM_TRACE_SERIAL_PORT
    return i;
}

size_t sys_write_serial_port(sys_serial_port_tp object, const char* data, size_t length) {

    if (!object) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_write_serial_port", "Serial port pointer is null");
        return 0;
    }
    if (length < 1) return length;

    if (!object->handle) {
        if (object->opened && object->configured) {
            object->handle = winapi_open_serial(object->path, object->baudrate, object->databits, object->stopbits, object->parity, 0);
            if (!object->handle) return 0;
        } else return 0;
    }

    // No of bytes written to the port
    DWORD dNoOfBytesWritten = 0;

    BOOL result = WriteFile(
        object->handle,
        data,
        length,
        &dNoOfBytesWritten,
        NULL
    );

    if (!result) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "sys_write_serial_port", "WriteFile failed with %d", (int)GetLastError());
        if (object->handle) {
            CloseHandle(object->handle);
            object->handle = NULL;
        }
        return 0;
    }

    #ifdef SYSTEM_TRACE_SERIAL_PORT
    sys_trace_array("sys_write_serial_port", data, dNoOfBytesWritten);
    #endif // SYSTEM_TRACE_SERIAL_PORT

    return dNoOfBytesWritten;
}
