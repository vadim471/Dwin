#include <modbus/master.hpp>
#include <initp/system/time.hpp>
#include "serial.hpp"

#include <signal.h>
#include <thread>
#include <chrono>

#if defined(WIN32)
#define DEFAULT_PORT "COM7"
#elif defined(POSIX)
#define DEFAULT_PORT "/dev/pts/1"
#endif // WIN32|POSIX

uint8_t started = 0;

void on_signal(int) { started = 0; }

extern "C" void mbs_free_request_eparam(size_t address) {
    // Nothing need to do
}

extern "C" void mbs_free_response_eparam(size_t address) {
    mbs_free((void*)address);
}

void on_write_single_coil(
    modbus::master& master,
    modbus::device& device,
    mbs_byte_t error
);

mbs_error_code_t write_single_coil(
    modbus::master& master,
    modbus::device& device,
    uint16_t address,
    uint8_t* value
);

mbs_error_code_t read_discrets(
    modbus::master& master,
    modbus::device& device,
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
            device.write_single_coil(1, modbus::repeat::once(), &write_single_coil, &on_write_single_coil);
        } else {
            mbs_debug_print(MBS_ERRC_TRACE, "read_discrets> Failed");
        }
    }
    return MBS_ERRC_NONE;
}

void on_write_multiple_coils(
    modbus::master& master,
    modbus::device& device,
    mbs_byte_t error
) {
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "on_write_multiple_coils> Error occurred: %d", error);
    } else {
        device.read_discrete_inputs(2, 3, modbus::repeat::once(), &read_discrets);
    }
}

mbs_error_code_t write_multiple_coils(
    modbus::master& master,
    modbus::device& device,
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
    modbus::master& master,
    modbus::device& device,
    mbs_byte_t error,
    uint16_t address,
    uint16_t count,
    const uint16_t* data
) {
    if (error) {
        mbs_debug_print(MBS_ERRC_GENERAL, "read_input> Error occurred: %d", error);
    } else {
        if ((address == 2) && (count == 3) && (data[0] == 10) && (data[1] == 20) && (data[2] == 30)) {
            device.write_multiple_coils(2, 3, modbus::repeat::once(), &write_multiple_coils, &on_write_multiple_coils);
        } else {
            mbs_debug_print(MBS_ERRC_TRACE, "read_input> Failed");
        }
    }
    return MBS_ERRC_NONE;
}

void on_write_multiple_registers(
    modbus::master& master,
    modbus::device& device,
    mbs_byte_t error
) {
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "on_write_multiple_registers> Error occurred: %d", error);
    } else {
        device.read_input_registers(2, 3, modbus::repeat::once(), &read_input);
    }
}

mbs_error_code_t write_multiple_registers(
    modbus::master& master,
    modbus::device& device,
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
    modbus::master& master,
    modbus::device& device,
    mbs_byte_t error,
    uint16_t address,
    uint16_t count,
    const uint16_t* registers
) {
    if (error) {
        mbs_debug_print(MBS_ERRC_GENERAL, "read_holding> Error occurred: %d", error);
    } else {
        if (registers[0] == 2) {
            device.write_multiple_registers(2, 3, modbus::repeat::once(), &write_multiple_registers, &on_write_multiple_registers);
        } else {
            mbs_debug_print(MBS_ERRC_TRACE, "read_holding> Failed");
        }
    }
    return MBS_ERRC_NONE;
}

void on_write_single_register(
    modbus::master& master,
    modbus::device& device,
    mbs_byte_t error
) {
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "on_write_single_register> Error occurred: %d", error);
    } else {
        device.read_holding_registers(1, 1, modbus::repeat::once(), &read_holding);
    }
}

mbs_error_code_t write_single_register(
    modbus::master& master,
    modbus::device& device,
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
    modbus::master& master,
    modbus::device& device,
    mbs_byte_t error,
    uint16_t address,
    uint16_t count,
    const uint8_t* data
) {
    if (error) {
        mbs_debug_print(MBS_ERRC_GENERAL, "read_coils> Error occurred: %d", error);
    } else {
        if ((address == 1) && (count == 1) && data[0]) {
            device.write_single_register(1, modbus::repeat::once(), &write_single_register, &on_write_single_register);
        } else {
            mbs_debug_print(MBS_ERRC_TRACE, "read_coils> Failed");
        }
    }
    return MBS_ERRC_NONE;
}

void on_write_single_coil(
    modbus::master& master,
    modbus::device& device,
    mbs_byte_t error
) {
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "on_write_single_coil> Error occurred: %d", error);
    } else {
        device.read_coils(1, 1, modbus::repeat::once(), &read_coils);
    }
}

mbs_error_code_t write_single_coil(
    modbus::master& master,
    modbus::device& device,
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

    modbus::master::ptr master = std::make_shared<modbus::master>();
    {
        serial::uptr endpoint(new serial());
        if (!endpoint->open((argc > 1) ? argv[1] : DEFAULT_PORT, 19200, 8, 1, 0)) {
            mbs_debug_print(MBS_ERRC_EP_NOT_CONFIGURED, "Failed to open serial port");
            return 0;
        } else master->endpoint(std::move(endpoint));
    }

    auto dev = master->create_device(1);
    if (std::get<1>(dev)) {
        mbs_debug_print(MBS_ERRC_TRACE, "Device NOT created, error: %d", std::get<1>(dev));
        return 0;
    }

    mbs_error_code_t error = std::get<0>(dev).write_single_coil(1, modbus::repeat::once(), &write_single_coil, &on_write_single_coil);
    if (error) {
        mbs_debug_print(MBS_ERRC_TRACE, "Failed to add task: %d", error);
    }

    for (started = 1; started;) {
        master->poll(initp::system::time::now());
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }
    return 0;
}
