#include <modbus/master.h>
#include <modbus/slave.h>
#include <modbus/debug.h>
#include <modbus/stack_trace.h>

#include <initp/system/time.h>

#include "serial.h"

#include <stdio.h>
#include <signal.h>

#if defined(WIN32)
#define DEFAULT_PORT "COM7"
#elif defined(POSIX)
#define DEFAULT_PORT "/dev/pts/1"
#endif // WIN32|POSIX

uint8_t started = 0;

void on_signal(int signal) { started = 0; }

void on_write_single_coil(
    mbs_master_tp master,
    mbs_device_tp device,
    mbs_byte_t error
);

mbs_error_code_t write_single_coil(
    mbs_master_tp master,
    mbs_device_tp device,
    uint16_t address,
    uint8_t* value
);

mbs_error_code_t read_discrets(
    mbs_master_tp master,
    mbs_device_tp device,
    mbs_byte_t error,
    uint16_t address,
    uint16_t count,
    const uint8_t* data
) {
    if (error) {
        mbs_debug_print(MBS_ERRC_GENERAL, "read_discrets> Error occurred: %d", error);
    } else {
        if ((address == 2) && (count == 3) && !data[0] && data[1] && !data[2]) {
            mbs_debug_print(MBS_ERRC_TRACE, "read_discrets> One step over");
            mbs_write_single_coil_once(device, 1, &write_single_coil, &on_write_single_coil);
        } else {
            mbs_debug_print(MBS_ERRC_TRACE, "read_discrets> Failed");
        }
    }
    return MBS_ERRC_NONE;
}

void on_write_multiple_coils(
    mbs_master_tp master,
    mbs_device_tp device,
    mbs_byte_t error
) {
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "on_write_multiple_coils> Error occurred: %d", error);
    } else {
        mbs_read_discrete_inputs_once(device, 2, 3, &read_discrets);
    }
}

mbs_error_code_t write_multiple_coils(
    mbs_master_tp master,
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    uint8_t* values
) {
    if ((address == 2) && (count == 3)) {
        values[0] = 1;
        values[1] = 0;
        values[2] = 1;
    } else {
        mbs_debug_print(MBS_ERRC_TRACE, "write_multiple_coils> Failed");
    }
    return MBS_ERRC_NONE;
}

mbs_error_code_t read_input(
    mbs_master_tp master,
    mbs_device_tp device,
    mbs_byte_t error,
    uint16_t address,
    uint16_t count,
    const uint16_t* data
) {
    if (error) {
        mbs_debug_print(MBS_ERRC_GENERAL, "read_input> Error occurred: %d", error);
    } else {
        if ((address == 2) && (count == 3) && (data[0] == 10) && (data[1] == 20) && (data[2] == 30)) {
            mbs_write_multiple_coils_once(device, 2, 3, &write_multiple_coils, &on_write_multiple_coils);
        } else {
            mbs_debug_print(MBS_ERRC_TRACE, "read_input> Failed");
        }
    }
    return MBS_ERRC_NONE;
}

void on_write_multiple_registers(
    mbs_master_tp master,
    mbs_device_tp device,
    mbs_byte_t error
) {
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "on_write_multiple_registers> Error occurred: %d", error);
    } else {
        mbs_read_input_registers_once(device, 2, 3, &read_input);
    }
}

mbs_error_code_t write_multiple_registers(
    mbs_master_tp master,
    mbs_device_tp device,
    uint16_t address,
    uint16_t count,
    uint16_t* values
) {
    if ((address == 2) && (count == 3)) {
        values[0] = 11;
        values[1] = 22;
        values[2] = 33;
    } else {
        mbs_debug_print(MBS_ERRC_TRACE, "write_multiple_registers> Failed");
    }
    return MBS_ERRC_NONE;
}

mbs_error_code_t read_holding(
    mbs_master_tp master,
    mbs_device_tp device,
    mbs_byte_t error,
    uint16_t address,
    uint16_t count,
    const uint16_t* registers
) {
    if (error) {
        mbs_debug_print(MBS_ERRC_GENERAL, "read_holding> Error occurred: %d", error);
    } else {
        if (registers[0] == 2) {
            mbs_write_multiple_registers_once(device, 2, 3, &write_multiple_registers, &on_write_multiple_registers);
        } else {
            mbs_debug_print(MBS_ERRC_TRACE, "read_holding> Failed");
        }
    }
    return MBS_ERRC_NONE;
}

void on_write_single_register(
    mbs_master_tp master,
    mbs_device_tp device,
    mbs_byte_t error
) {
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "on_write_single_register> Error occurred: %d", error);
    } else {
        mbs_read_holding_registers_once(device, 1, 1, &read_holding);
    }
}

mbs_error_code_t write_single_register(
    mbs_master_tp master,
    mbs_device_tp device,
    uint16_t address,
    uint16_t* value
) {
    if (address == 1) {
        *value = 1;
    } else {
        mbs_debug_print(MBS_ERRC_TRACE, "write_single_register> Failed");
    }
    return MBS_ERRC_NONE;
}

mbs_error_code_t read_coils(
    mbs_master_tp master,
    mbs_device_tp device,
    mbs_byte_t error,
    uint16_t address,
    uint16_t count,
    const uint8_t* data
) {
    if (error) {
        mbs_debug_print(MBS_ERRC_GENERAL, "read_coils> Error occurred: %d", error);
    } else {
        if ((address == 1) && (count == 1) && data[0]) {
            mbs_write_single_register_once(device, 1, &write_single_register, &on_write_single_register);
        } else {
            mbs_debug_print(MBS_ERRC_TRACE, "read_coils> Failed");
        }
    }
    return MBS_ERRC_NONE;
}

void on_write_single_coil(
    mbs_master_tp master,
    mbs_device_tp device,
    mbs_byte_t error
) {
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "on_write_single_coil> Error occurred: %d", error);
    } else {
        mbs_read_coils_once(device, 1, 1, &read_coils);
    }
}

mbs_error_code_t write_single_coil(
    mbs_master_tp master,
    mbs_device_tp device,
    uint16_t address,
    uint8_t* value
) {
    if (address == 1) {
        *value = 1;
    } else {
        mbs_debug_print(MBS_ERRC_TRACE, "write_single_coil> Failed");
    }
    return MBS_ERRC_NONE;
}

int main(int argc, char** argv) {

    signal(SIGINT, &on_signal);
    signal(SIGTERM, &on_signal);

    mbs_init_stack_trace(argv[0]);

    sys_serial_port_t ep;
    sys_init_serial_port(&ep);
    if (!sys_open_serial_port(&ep, (argc > 1) ? argv[1] : DEFAULT_PORT, 19200, 8, 1, 0)) {
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

    error = mbs_write_single_coil_once(device, 1, &write_single_coil, &on_write_single_coil);
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "Failed to add task: %d", error);
    }

    for (started = 1; started;) {
        mbs_poll_master(&master, sys_clock_ms());
        sys_sleep_for(50);
    }

    mbs_dispose_master(&master);
    sys_close_serial_port(&ep);
    return 0;
}
