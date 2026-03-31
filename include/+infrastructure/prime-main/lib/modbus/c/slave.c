#include <modbus/slave.h>
#include <modbus/debug.h>
#include <modbus/stack_trace.h>

#include <initp/system/time.h>

#include "serial.h"

#include <stdio.h>
#include <signal.h>

#if defined(WIN32)
#define DEFAULT_PORT "COM6"
#elif defined(POSIX)
#define DEFAULT_PORT "/dev/pts/2"
#endif // WIN32|POSIX

uint8_t started = 0;

void on_signal(int signal) { started = 0; }

uint8_t coils_234[3];

mbs_byte_t on_read_discrets(
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    uint8_t* data
) {
    //mbs_debug_print(MBS_ERRC_TRACE, "on_read_discrets> Called");
    if ((address == 2) && (count == 3)) {
        data[0] = coils_234[0];
        data[1] = coils_234[1];
        data[2] = coils_234[2];
    } else {
        mbs_debug_print(MBS_ERRC_TRACE, "on_read_discrets> Failed");
    }
    return MBS_ERR_NONE;
}

mbs_byte_t on_write_coils(
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    const uint8_t* data
) {
    //mbs_debug_print(MBS_ERRC_TRACE, "on_write_coils> Called");
    if ((address == 2) && (count == 3) && data[0] && !data[1] && data[2]) {
        coils_234[0] = 0;
        coils_234[1] = 1;
        coils_234[2] = 0;
    } else {
        coils_234[0] = 0;
        coils_234[1] = 0;
        coils_234[2] = 0;
        mbs_debug_print(MBS_ERRC_TRACE, "on_write_coils> Failed");
    }
    return MBS_ERR_NONE;
}

uint16_t regs_234[3];

mbs_byte_t on_read_input(
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    uint16_t* data
) {
    //mbs_debug_print(MBS_ERRC_TRACE, "on_read_input> Called");
    if ((address == 2) && (count == 3)) {
        data[0] = regs_234[0];
        data[1] = regs_234[1];
        data[2] = regs_234[2];
    } else {
        mbs_debug_print(MBS_ERRC_TRACE, "on_read_input> Failed");
    }
    return MBS_ERR_NONE;
}

mbs_byte_t on_write_registers(
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    const uint16_t* data
) {
    //mbs_debug_print(MBS_ERRC_TRACE, "on_write_registers> Called");
    if ((address == 2) && (count == 3) && (data[0] == 11) && (data[1] == 22) && (data[2] == 33)) {
        regs_234[0] = 10;
        regs_234[1] = 20;
        regs_234[2] = 30;
    } else {
        regs_234[0] = 0;
        regs_234[1] = 0;
        regs_234[2] = 0;
        mbs_debug_print(MBS_ERRC_TRACE, "on_write_registers> Failed");
    }
    return MBS_ERR_NONE;
}

uint16_t reg_1;

mbs_byte_t on_read_holding(
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    uint16_t* data
) {
    //mbs_debug_print(MBS_ERRC_TRACE, "on_read_holding> Called");
    if ((address == 1) && (count == 1)) {
        *data = reg_1;
    } else {
        mbs_debug_print(MBS_ERRC_TRACE, "on_read_holding> Failed");
    }
    return MBS_ERR_NONE;
}

mbs_byte_t on_write_register(
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    const uint16_t* data
) {
    //mbs_debug_print(MBS_ERRC_TRACE, "on_write_register> Called");
    if ((address == 1) && (count == 1) && (data[0] == 1)) {
        reg_1 = 2;
    } else {
        reg_1 = 0;
        mbs_debug_print(MBS_ERRC_TRACE, "on_write_register> Failed");
    }
    return MBS_ERR_NONE;
}

uint8_t coil_1;

mbs_byte_t on_read_coils(
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    uint8_t* data
) {
    //mbs_debug_print(MBS_ERRC_TRACE, "on_read_coils> Called");
    if ((address == 1) && (count == 1)) {
        *data = coil_1;
    } else {
        mbs_debug_print(MBS_ERRC_TRACE, "on_read_coils> Failed");
    }
    return MBS_ERR_NONE;
}

mbs_byte_t on_write_coil(
    mbs_slave_tp slave,
    uint16_t address,
    uint16_t count,
    const uint8_t* data
) {
    //mbs_debug_print(MBS_ERRC_TRACE, "on_write_coil> Called");
    if ((address == 1) && (count == 1) && data[0]) {
        coil_1 = 1;
    } else {
        coil_1 = 0;
        mbs_debug_print(MBS_ERRC_TRACE, "on_write_coil> Failed");
    }
    return MBS_ERR_NONE;
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

    mbs_slave_t slave;
    mbs_init_slave_rtu(&slave, 1, &endpoint);

    mbs_error_code_t error = mbs_on_read_coils(&slave, 1, 10, &on_read_coils);
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "Failed to set handler: %d", error);
    }

    error = mbs_on_read_discrete_inputs(&slave, 1, 10, &on_read_discrets);
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "Failed to set handler: %d", error);
    }

    error = mbs_on_read_holding_registers(&slave, 1, 10, &on_read_holding);
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "Failed to set handler: %d", error);
    }

    error = mbs_on_read_input_registers(&slave, 1, 10, &on_read_input);
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "Failed to set handler: %d", error);
    }

    error = mbs_on_write_multiple_coils(&slave, 1, 1, &on_write_coil);
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "Failed to set handler: %d", error);
    }

    error = mbs_on_write_multiple_coils(&slave, 2, 9, &on_write_coils);
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "Failed to set handler: %d", error);
    }

    error = mbs_on_write_multiple_registers(&slave, 1, 1, &on_write_register);
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "Failed to set handler: %d", error);
    }

    error = mbs_on_write_multiple_registers(&slave, 2, 9, &on_write_registers);
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "Failed to set handler: %d", error);
    }

    for (started = 1; started;) {
        mbs_poll_slave(&slave, sys_clock_ms());
        sys_sleep_for(10);
    }

    mbs_dispose_slave(&slave);
    sys_close_serial_port(&ep);
    return 0;
}
