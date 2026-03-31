#ifndef MODBUS_DEVICE_HPP_INCLUDED
#define MODBUS_DEVICE_HPP_INCLUDED

#include "callback.hpp"
#include "repeat.hpp"

extern "C" {

#include <modbus/types.h>

}

namespace modbus {

class master;
class device {
    friend class master;

private: // Private types
    typedef device self_type;

public: // Public types
    typedef std::unique_ptr<self_type> uptr;

private: // Construction
    device(master& parent, mbs_device_tp c_object);

public: // Construction
    device(const self_type&) = delete;
    virtual ~device(void) = default;

public: // Task methods
    mbs_error_code_t read_coils(
        uint16_t address,
        uint16_t count,
        repeat::uptr&& repeat,
        on_master_read_coils_f on_response,
        uint8_t priority = 0
    );
    mbs_error_code_t read_discrete_inputs(
        uint16_t address,
        uint16_t count,
        repeat::uptr&& repeat,
        on_master_read_discret_inputs_f on_response,
        uint8_t priority = 0
    );
    mbs_error_code_t read_holding_registers(
        uint16_t address,
        uint16_t count,
        repeat::uptr&& repeat,
        on_master_read_holding_registers_f on_response,
        uint8_t priority = 0
    );
    mbs_error_code_t read_input_registers(
        uint16_t address,
        uint16_t count,
        repeat::uptr&& repeat,
        on_master_read_input_registers_f on_response,
        uint8_t priority = 0
    );
    mbs_error_code_t write_single_coil(
        uint16_t address,
        repeat::uptr&& repeat,
        on_master_write_single_coil_f on_request,
        on_master_result_f on_response,
        uint8_t priority = 0
    );
    mbs_error_code_t write_single_register(
        uint16_t address,
        repeat::uptr&& repeat,
        on_master_write_single_register_f on_request,
        on_master_result_f on_response,
        uint8_t priority = 0
    );
    mbs_error_code_t write_multiple_coils(
        uint16_t address,
        uint16_t count,
        repeat::uptr&& repeat,
        on_master_write_multiple_coils_f on_request,
        on_master_result_f on_response,
        uint8_t priority = 0
    );
    mbs_error_code_t write_multiple_registers(
        uint16_t address,
        uint16_t count,
        repeat::uptr&& repeat,
        on_master_write_multiple_registers_f on_request,
        on_master_result_f on_response,
        uint8_t priority = 0
    );

private: // Handler calls
    void call_on_result(mbs_byte_t error, on_master_result_f on_result);
    mbs_error_code_t call_on_read_coils(mbs_byte_t error, uint16_t address, uint16_t count, const uint8_t* coils, on_master_read_coils_f on_response);
    mbs_error_code_t call_on_read_holding_registers(mbs_byte_t error, uint16_t address, uint16_t count, const uint16_t* registers, on_master_read_holding_registers_f on_response);
    mbs_error_code_t call_on_write_single_coil(uint16_t address, uint8_t* value, on_master_write_single_coil_f on_request);
    mbs_error_code_t call_on_write_single_register(uint16_t address, uint16_t* value, on_master_write_single_register_f on_request);
    mbs_error_code_t call_on_write_multiple_coils(uint16_t address, uint16_t count, uint8_t* value, on_master_write_multiple_coils_f on_request);
    mbs_error_code_t call_on_write_multiple_registers(uint16_t address, uint16_t count, uint16_t* value, on_master_write_multiple_registers_f on_request);

public: // Public properties
    mbs_device_tp c_object(void);
    uint8_t address(void) const;

private: // Private properties
    void c_object(mbs_device_tp c_object);

private: // Private fields
    mbs_device_tp c_object_;
    master& parent_;
};

}

#endif // MODBUS_DEVICE_HPP_INCLUDED
