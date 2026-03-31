#ifndef MODBUS_SLAVE_HPP_INCLUDED
#define MODBUS_SLAVE_HPP_INCLUDED

#include "config.hpp"
#include "callback.hpp"
#include "endpoint.hpp"

extern "C" {

#include <modbus/types.h>
#include <modbus/symbol.h>

}

#include <memory>

#ifdef MODBUS_ENABLE_THREAD_SAFETY
#include <mutex>
#endif // MODBUS_ENABLE_THREAD_SAFETY

namespace modbus {

class slave:
    public std::enable_shared_from_this<slave> {

private: // Private types
    typedef slave self_type;

public: // Public types
    typedef std::shared_ptr<self_type> ptr;
    typedef std::unique_ptr<self_type> uptr;
    typedef std::weak_ptr<self_type> wptr;

public: // Construction
    slave(uint8_t id);
    slave(const self_type&) = delete;
    virtual ~slave(void);

public: // Public methods
    virtual void poll(mbs_time_t time);
    void dispose(void);
    void reinitialize(uint8_t id);
    void endpoint(endpoint::uptr object);

public: // Handlers register
    mbs_error_code_t on_read_coils(uint16_t address, uint16_t count, on_slave_read_coils_f on_request);
    mbs_error_code_t on_read_discrete_inputs(uint16_t address, uint16_t count, on_slave_read_discrete_inputs_f on_request);
    mbs_error_code_t on_read_holding_registers(uint16_t address, uint16_t count, on_slave_read_holding_registers_f on_request);
    mbs_error_code_t on_read_input_registers(uint16_t address, uint16_t count, on_slave_read_input_registers_f on_request);
    mbs_error_code_t on_write_multiple_coils(uint16_t address, uint16_t count, on_slave_write_multiple_coils_f on_request);
    mbs_error_code_t on_write_multiple_registers(uint16_t address, uint16_t count, on_slave_write_multiple_registers_f on_request);

private: // Handler calls
    mbs_byte_t call_on_read_coils(uint16_t address, uint16_t count, uint8_t* coils, on_slave_read_coils_f on_request);
    mbs_byte_t call_on_read_holding_registers(uint16_t address, uint16_t count, uint16_t* registers, on_slave_read_holding_registers_f on_request);
    mbs_byte_t call_on_write_multiple_coils(uint16_t address, uint16_t count, const uint8_t* coils, on_slave_write_multiple_coils_f on_request);
    mbs_byte_t call_on_write_multiple_registers(uint16_t address, uint16_t count, const uint16_t* registers, on_slave_write_multiple_registers_f on_request);

public: // Public properties
    mbs_slave_tp c_object(void);
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::recursive_mutex& mutex(void);
    #endif // MODBUS_ENABLE_THREAD_SAFETY

private: // Private fields
    mbs_slave_tp c_object_;
    endpoint::uptr endpoint_;
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::recursive_mutex mutex_;
    #endif // MODBUS_ENABLE_THREAD_SAFETY
};

}

#endif // MODBUS_SLAVE_HPP_INCLUDED
