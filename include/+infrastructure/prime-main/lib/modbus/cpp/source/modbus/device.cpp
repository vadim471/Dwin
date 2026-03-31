#include <modbus/device.hpp>

extern "C" {

#include <modbus/device.h>
#include <modbus/callback.h>
#include <modbus/task.h>

}

#include <initp/generic/function_generic.hpp>
#include <initp/system/macro.hpp>

namespace modbus {

    // Construction

device::device(master& parent, mbs_device_tp c_object):
    c_object_(c_object),
    parent_(parent)
{}

    // Task methods

mbs_error_code_t device::read_coils(
    uint16_t address,
    uint16_t count,
    repeat::uptr&& repeat,
    on_master_read_coils_f on_response,
    uint8_t priority
) {
    using namespace std::placeholders;
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    if (on_response) {
        auto f = initp::tools::function<mbs_master_read_coils_f>::bind(RBIND(call_on_read_coils, _3, _4, _5, _6, on_response));
        mbs_error_code_t errc = mbs_read_coils(this->c_object_, address, count, &repeat_check, &repeat_reset, repeat.release(), priority, std::get<0>(f), std::get<1>(f));
        if (errc) initp::tools::function_pointer<mbs_master_read_coils_fp>::free(std::get<1>(f));
        return errc;
    } else return mbs_read_coils(this->c_object_, address, count, &repeat_check, &repeat_reset, repeat.release(), priority, NULL, 0);
}

mbs_error_code_t device::read_discrete_inputs(
    uint16_t address,
    uint16_t count,
    repeat::uptr&& repeat,
    on_master_read_discret_inputs_f on_response,
    uint8_t priority
) {
    using namespace std::placeholders;
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    if (on_response) {
        auto f = initp::tools::function<mbs_master_read_coils_f>::bind(RBIND(call_on_read_coils, _3, _4, _5, _6, on_response));
        mbs_error_code_t errc = mbs_read_discrete_inputs(this->c_object_, address, count, &repeat_check, &repeat_reset, repeat.release(), priority, std::get<0>(f), std::get<1>(f));
        if (errc) initp::tools::function_pointer<mbs_master_read_coils_fp>::free(std::get<1>(f));
        return errc;
    } else return mbs_read_discrete_inputs(this->c_object_, address, count, &repeat_check, &repeat_reset, repeat.release(), priority, NULL, 0);
}

mbs_error_code_t device::read_holding_registers(
    uint16_t address,
    uint16_t count,
    repeat::uptr&& repeat,
    on_master_read_holding_registers_f on_response,
    uint8_t priority
) {
    using namespace std::placeholders;
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    if (on_response) {
        auto f = initp::tools::function<mbs_master_read_holding_registers_f>::bind(RBIND(call_on_read_holding_registers, _3, _4, _5, _6, on_response));
        mbs_error_code_t errc = mbs_read_holding_registers(this->c_object_, address, count, &repeat_check, &repeat_reset, repeat.release(), priority, std::get<0>(f), std::get<1>(f));
        if (errc) initp::tools::function_pointer<mbs_master_read_holding_registers_fp>::free(std::get<1>(f));
        return errc;
    } else return mbs_read_holding_registers(this->c_object_, address, count, &repeat_check, &repeat_reset, repeat.release(), priority, NULL, 0);
}

mbs_error_code_t device::read_input_registers(
    uint16_t address,
    uint16_t count,
    repeat::uptr&& repeat,
    on_master_read_input_registers_f on_response,
    uint8_t priority
) {
    using namespace std::placeholders;
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    if (on_response) {
        auto f = initp::tools::function<mbs_master_read_holding_registers_f>::bind(RBIND(call_on_read_holding_registers, _3, _4, _5, _6, on_response));
        mbs_error_code_t errc = mbs_read_input_registers(this->c_object_, address, count, &repeat_check, &repeat_reset, repeat.release(), priority, std::get<0>(f), std::get<1>(f));
        if (errc) initp::tools::function_pointer<mbs_master_read_holding_registers_fp>::free(std::get<1>(f));
        return errc;
    } else return mbs_read_input_registers(this->c_object_, address, count, &repeat_check, &repeat_reset, repeat.release(), priority, NULL, 0);
}

mbs_error_code_t device::write_single_coil(
    uint16_t address,
    repeat::uptr&& repeat,
    on_master_write_single_coil_f on_request,
    on_master_result_f on_response,
    uint8_t priority
) {
    using namespace std::placeholders;
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    mbs_master_result_fp handler = NULL;
    size_t eparam = 0;
    if (on_response) {
        std::tie(handler, eparam) = initp::tools::function<mbs_master_result_f>::bind(RBIND(call_on_result, _3, on_response));
    }
    if (on_request) {
        auto f = initp::tools::function<mbs_master_write_single_coil_f>::bind(RBIND(call_on_write_single_coil, _3, _4, on_request));
        mbs_error_code_t errc = mbs_write_single_coil(this->c_object_, address, &repeat_check, &repeat_reset, repeat.release(), priority, std::get<0>(f), std::get<1>(f), handler, eparam);
        if (errc) {
            initp::tools::function_pointer<mbs_master_write_single_coil_fp>::free(std::get<1>(f));
            if (on_response) initp::tools::function_pointer<mbs_master_result_fp>::free(eparam);
        }
        return errc;
    } else {
        mbs_error_code_t errc = mbs_write_single_coil(this->c_object_, address, &repeat_check, &repeat_reset, repeat.release(), priority, NULL, 0, handler, eparam);
        if (errc && on_response) initp::tools::function_pointer<mbs_master_result_fp>::free(eparam);
        return errc;
    }
}

mbs_error_code_t device::write_single_register(
    uint16_t address,
    repeat::uptr&& repeat,
    on_master_write_single_register_f on_request,
    on_master_result_f on_response,
    uint8_t priority
) {
    using namespace std::placeholders;
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    mbs_master_result_fp handler = NULL;
    size_t eparam = 0;
    if (on_response) {
        std::tie(handler, eparam) = initp::tools::function<mbs_master_result_f>::bind(RBIND(call_on_result, _3, on_response));
    }
    if (on_request) {
        auto f = initp::tools::function<mbs_master_write_single_register_f>::bind(RBIND(call_on_write_single_register, _3, _4, on_request));
        mbs_error_code_t errc = mbs_write_single_register(this->c_object_, address, &repeat_check, &repeat_reset, repeat.release(), priority, std::get<0>(f), std::get<1>(f), handler, eparam);
        if (errc) {
            initp::tools::function_pointer<mbs_master_write_single_register_fp>::free(std::get<1>(f));
            if (on_response) initp::tools::function_pointer<mbs_master_result_fp>::free(eparam);
        }
        return errc;
    } else {
        mbs_error_code_t errc = mbs_write_single_register(this->c_object_, address, &repeat_check, &repeat_reset, repeat.release(), priority, NULL, 0, handler, eparam);
        if (errc && on_response) initp::tools::function_pointer<mbs_master_result_fp>::free(eparam);
        return errc;
    }
}

mbs_error_code_t device::write_multiple_coils(
    uint16_t address,
    uint16_t count,
    repeat::uptr&& repeat,
    on_master_write_multiple_coils_f on_request,
    on_master_result_f on_response,
    uint8_t priority
) {
    using namespace std::placeholders;
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    mbs_master_result_fp handler = NULL;
    size_t eparam = 0;
    if (on_response) {
        std::tie(handler, eparam) = initp::tools::function<mbs_master_result_f>::bind(RBIND(call_on_result, _3, on_response));
    }
    if (on_request) {
        auto f = initp::tools::function<mbs_master_write_multiple_coils_f>::bind(RBIND(call_on_write_multiple_coils, _3, _4, _5, on_request));
        mbs_error_code_t errc = mbs_write_multiple_coils(this->c_object_, address, count, &repeat_check, &repeat_reset, repeat.release(), priority, std::get<0>(f), std::get<1>(f), handler, eparam);
        if (errc) {
            initp::tools::function_pointer<mbs_master_write_multiple_coils_fp>::free(std::get<1>(f));
            if (on_response) initp::tools::function_pointer<mbs_master_result_fp>::free(eparam);
        }
        return errc;
    } else {
        mbs_error_code_t errc = mbs_write_multiple_coils(this->c_object_, address, count, &repeat_check, &repeat_reset, repeat.release(), priority, NULL, 0, handler, eparam);
        if (errc && on_response) initp::tools::function_pointer<mbs_master_result_fp>::free(eparam);
        return errc;
    }
}

mbs_error_code_t device::write_multiple_registers(
    uint16_t address,
    uint16_t count,
    repeat::uptr&& repeat,
    on_master_write_multiple_registers_f on_request,
    on_master_result_f on_response,
    uint8_t priority
) {
    using namespace std::placeholders;
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    mbs_master_result_fp handler = NULL;
    size_t eparam = 0;
    if (on_response) {
        std::tie(handler, eparam) = initp::tools::function<mbs_master_result_f>::bind(RBIND(call_on_result, _3, on_response));
    }
    if (on_request) {
        auto f = initp::tools::function<mbs_master_write_multiple_registers_f>::bind(RBIND(call_on_write_multiple_registers, _3, _4, _5, on_request));
        mbs_error_code_t errc = mbs_write_multiple_registers(this->c_object_, address, count, &repeat_check, &repeat_reset, repeat.release(), priority, std::get<0>(f), std::get<1>(f), handler, eparam);
        if (errc) {
            initp::tools::function_pointer<mbs_master_write_multiple_registers_fp>::free(std::get<1>(f));
            if (on_response) initp::tools::function_pointer<mbs_master_result_fp>::free(eparam);
        }
        return errc;
    } else {
        mbs_error_code_t errc = mbs_write_multiple_registers(this->c_object_, address, count, &repeat_check, &repeat_reset, repeat.release(), priority, NULL, 0, handler, eparam);
        if (errc && on_response) initp::tools::function_pointer<mbs_master_result_fp>::free(eparam);
        return errc;
    }
}

    // Handler calls

void device::call_on_result(mbs_byte_t error, on_master_result_f on_result) {
    if (!on_result) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "modbus::master::call_on_result> No on_result callback");
        return;
    }
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    on_result(this->parent_, *this, error);
}

mbs_error_code_t device::call_on_read_coils(mbs_byte_t error, uint16_t address, uint16_t count, const uint8_t* coils, on_master_read_coils_f on_response) {
    if (!on_response) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "modbus::master::call_on_read_coils> No on_response callback");
        return MBS_ERRC_NULL_POINTER;
    }
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    return on_response(this->parent_, *this, error, address, count, coils);
}

mbs_error_code_t device::call_on_read_holding_registers(mbs_byte_t error, uint16_t address, uint16_t count, const uint16_t* registers, on_master_read_holding_registers_f on_response) {
    if (!on_response) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "modbus::master::call_on_read_holding_registers> No on_response callback");
        return MBS_ERRC_NULL_POINTER;
    }
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    return on_response(this->parent_, *this, error, address, count, registers);
}

mbs_error_code_t device::call_on_write_single_coil(uint16_t address, uint8_t* value, on_master_write_single_coil_f on_request) {
    if (!on_request) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "modbus::master::call_on_write_single_coil> No on_request callback");
        return MBS_ERRC_NULL_POINTER;
    }
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    return on_request(this->parent_, *this, address, value);
}

mbs_error_code_t device::call_on_write_single_register(uint16_t address, uint16_t* value, on_master_write_single_register_f on_request) {
    if (!on_request) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "modbus::master::call_on_write_single_register> No on_request callback");
        return MBS_ERRC_NULL_POINTER;
    }
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    return on_request(this->parent_, *this, address, value);
}

mbs_error_code_t device::call_on_write_multiple_coils(uint16_t address, uint16_t count, uint8_t* value, on_master_write_multiple_coils_f on_request) {
    if (!on_request) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "modbus::master::call_on_write_multiple_coils> No on_request callback");
        return MBS_ERRC_NULL_POINTER;
    }
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    return on_request(this->parent_, *this, address, count, value);
}

mbs_error_code_t device::call_on_write_multiple_registers(uint16_t address, uint16_t count, uint16_t* value, on_master_write_multiple_registers_f on_request) {
    if (!on_request) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "modbus::master::call_on_write_multiple_registers> No on_request callback");
        return MBS_ERRC_NULL_POINTER;
    }
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    return on_request(this->parent_, *this, address, count, value);
}

    // Public properties

mbs_device_tp device::c_object(void) {
    return this->c_object_;
}

uint8_t device::address(void) const {
    if (!this->c_object_) return 0;
    return this->c_object_->address;
}

    // Private properties

void device::c_object(mbs_device_tp c_object) {
    this->c_object_ = c_object;
}

}
