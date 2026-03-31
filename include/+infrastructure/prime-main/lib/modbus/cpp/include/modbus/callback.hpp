#ifndef MODBUS_CALLBACK_HPP_INCLUDED
#define MODBUS_CALLBACK_HPP_INCLUDED

#include "config.hpp"

extern "C" {

#include <modbus/error_code.h>

}

#include <functional>

namespace modbus {

class master;
class device;
class slave;

typedef std::function<
    void(master&, device&, mbs_byte_t)
> on_master_result_f;

typedef std::function<
    mbs_error_code_t(master&, device&, mbs_byte_t, uint16_t, uint16_t, const uint8_t*)
> on_master_read_coils_f;

typedef on_master_read_coils_f on_master_read_discret_inputs_f;

typedef std::function<
    mbs_error_code_t(master&, device&, mbs_byte_t, uint16_t, uint16_t, const uint16_t*)
> on_master_read_holding_registers_f;

typedef on_master_read_holding_registers_f on_master_read_input_registers_f;

typedef std::function<
    mbs_error_code_t(master&, device&, uint16_t, uint8_t*)
> on_master_write_single_coil_f;

typedef std::function<
    mbs_error_code_t(master&, device&, uint16_t, uint16_t*)
> on_master_write_single_register_f;

typedef std::function<
    mbs_error_code_t(master&, device&, uint16_t, uint16_t, uint8_t*)
> on_master_write_multiple_coils_f;

typedef std::function<
    mbs_error_code_t(master&, device&, uint16_t, uint16_t, uint16_t*)
> on_master_write_multiple_registers_f;

typedef std::function<
    mbs_byte_t(slave&, uint16_t, uint16_t, uint8_t*)
> on_slave_read_coils_f;

typedef on_slave_read_coils_f on_slave_read_discrete_inputs_f;

typedef std::function<
    mbs_byte_t(slave&, uint16_t, uint16_t, uint16_t*)
> on_slave_read_holding_registers_f;

typedef on_slave_read_holding_registers_f on_slave_read_input_registers_f;

typedef std::function<
    mbs_byte_t(slave&, uint16_t, uint16_t, const uint8_t*)
> on_slave_write_multiple_coils_f;

typedef std::function<
    mbs_byte_t(slave&, uint16_t, uint16_t, const uint16_t*)
> on_slave_write_multiple_registers_f;

}

#endif // MODBUS_CALLBACK_HPP_INCLUDED
