#include <modbus/slave.hpp>
#include <modbus/memory.h>
#include <initp/system/time.hpp>
#include "serial.hpp"

#include <signal.h>
#include <thread>
#include <chrono>

#if defined(WIN32)
#define DEFAULT_PORT "COM6"
#elif defined(POSIX)
#define DEFAULT_PORT "/dev/pts/2"
#endif // WIN32|POSIX

uint8_t started = 0;

void on_signal(int) { started = 0; }

uint8_t coils_234[3];

extern "C" void mbs_free_request_eparam(size_t address) {
    // Nothing need to do
}

extern "C" void mbs_free_response_eparam(size_t address) {
    mbs_free((void*)address);
}

mbs_byte_t on_read_discrets(
    modbus::slave& slave,
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
    modbus::slave& slave,
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
    modbus::slave& slave,
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
    modbus::slave& slave,
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
    modbus::slave& slave,
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
    modbus::slave& slave,
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
    modbus::slave& slave,
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
    modbus::slave& slave,
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

    modbus::slave::ptr slave = std::make_shared<modbus::slave>(1);
    {
        serial::uptr endpoint(new serial());
        if (!endpoint->open((argc > 1) ? argv[1] : DEFAULT_PORT, 19200, 8, 1, 0)) {
            mbs_debug_print(MBS_ERRC_EP_NOT_CONFIGURED, "Failed to open serial port");
            return 0;
        } else slave->endpoint(std::move(endpoint));
    }

    mbs_error_code_t error = slave->on_read_coils(1, 10, &on_read_coils);
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "Failed to set handler: %d", error);
    }

    error = slave->on_read_discrete_inputs(1, 10, &on_read_discrets);
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "Failed to set handler: %d", error);
    }

    error = slave->on_read_holding_registers(1, 10, &on_read_holding);
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "Failed to set handler: %d", error);
    }

    error = slave->on_read_input_registers(1, 10, &on_read_input);
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "Failed to set handler: %d", error);
    }

    error = slave->on_write_multiple_coils(1, 1, &on_write_coil);
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "Failed to set handler: %d", error);
    }

    error = slave->on_write_multiple_coils(2, 9, &on_write_coils);
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "Failed to set handler: %d", error);
    }

    error = slave->on_write_multiple_registers(1, 1, &on_write_register);
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "Failed to set handler: %d", error);
    }

    error = slave->on_write_multiple_registers(2, 9, &on_write_registers);
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "Failed to set handler: %d", error);
    }

    for (started = 1; started;) {
        slave->poll(initp::system::time::now());
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }
    return 0;
}
