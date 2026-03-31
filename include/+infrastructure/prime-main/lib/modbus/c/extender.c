#include <modbus/master.h>
#include <modbus/slave.h>
#include <modbus/debug.h>
#include <modbus/stack_trace.h>

#include <initp/system/time.h>

#include "serial.h"

#include <stdio.h>
#include <signal.h>

#if defined(WIN32)
#define DEFAULT_PORT "COM30"
#elif defined(POSIX)
#define DEFAULT_PORT "/dev/pts/1"
#endif // WIN32|POSIX

uint8_t started = 0;

void on_signal(int signal) { started = 0; }

mbs_error_code_t on_read_discrets(
    mbs_master_tp master,
    mbs_device_tp device,
    mbs_byte_t error,
    uint16_t address,
    uint16_t count,
    const uint8_t* coils
) {
    mbs_debug_print(MBS_ERRC_TRACE, "on_read_discrets> Result: %d", (int)error);
    return MBS_ERRC_NONE;
}

int main(int argc, char** argv) {

    signal(SIGINT, &on_signal);
    signal(SIGTERM, &on_signal);

    mbs_init_stack_trace(argv[0]);

    sys_serial_port_t ep;
    sys_init_serial_port(&ep);
    if (!sys_open_serial_port(&ep, (argc > 1) ? argv[1] : DEFAULT_PORT, 38400, 8, 2, 0)) {
        printf("Failed to open serial port\r\n");
        return 0;
    }

    mbs_endpoint_t endpoint;
    mbs_init_endpoint(&endpoint, &mbs_read_serial, &mbs_write_serial, &ep);

    mbs_master_t master;
    mbs_init_master_rtu(&master, &endpoint);
    mbs_expand_master(&master, 1);

    mbs_device_tp device = NULL;
    mbs_error_code_t error = mbs_create_device(&master, 1, &device);
    if (!error) {
        mbs_debug_print(MBS_ERRC_TRACE, "Device created");
    } else mbs_debug_print(MBS_ERRC_TRACE, "Device NOT created, error: %d", error);

    error = mbs_read_discrete_inputs_loop(device, 0, 64, 1000, &on_read_discrets);
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "Failed to add task: %d", error);
    }

    for (started = 1; started;) {
        mbs_poll_master(&master, sys_clock_ms());
        sys_sleep_for(10);
    }

    mbs_dispose_master(&master);
    sys_close_serial_port(&ep);
    return 0;
}
