#include <modbus/slave.hpp>

extern "C" {

#include <modbus/slave.h>
#include <modbus/memory.h>

}

#include <initp/generic/function_generic.hpp>
#include <initp/system/macro.hpp>

#include <functional>

namespace modbus {

    // Construction

slave::slave(uint8_t id):
    c_object_(NULL) {
    this->c_object_ = (mbs_slave_tp)mbs_malloc(sizeof(mbs_slave_t), "modbus::slave:mbs_slave");
    if (this->c_object_) {
        mbs_init_slave_rtu(this->c_object_, id, NULL);
    }
    //std::cout << "modbus::slave::slave();" << std::endl;
}

slave::~slave(void) {
    if (this->c_object_) {
        mbs_free_slave(this->c_object_);
    }
    //std::cout << "modbus::slave::~slave();" << std::endl;
}

    // Public methods

void slave::poll(mbs_time_t time) {
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    mbs_poll_slave(this->c_object_, time);
}

void slave::dispose(void) {
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    mbs_dispose_slave(this->c_object_);
    this->endpoint_.reset();
}

void slave::reinitialize(uint8_t id) {
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    mbs_init_slave_rtu(this->c_object_, id, NULL);
}

void slave::endpoint(endpoint::uptr object) {
    using namespace std::placeholders;
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    if (object) {
        this->endpoint_ = std::move(object);
        this->c_object_->endpoint = this->endpoint_->c_object();
    } else if (this->endpoint_) {
        this->c_object_->endpoint = NULL;
        this->endpoint_.reset();
    }
}

    // Handlers register

mbs_error_code_t slave::on_read_coils(
    uint16_t address,
    uint16_t count,
    on_slave_read_coils_f on_request
) {
    using namespace std::placeholders;
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    if (on_request) {
        auto f = initp::tools::function<mbs_slave_read_coils_f>::bind(RBIND(call_on_read_coils, _2, _3, _4, on_request));
        mbs_error_code_t errc = mbs_on_read_coils(this->c_object_, address, count, std::get<0>(f), std::get<1>(f));
        if (errc) initp::tools::function_pointer<mbs_slave_read_coils_fp>::free(std::get<1>(f));
        return errc;
    } else return mbs_on_read_coils(this->c_object_, address, count, NULL, 0);
}

mbs_error_code_t slave::on_read_discrete_inputs(
    uint16_t address,
    uint16_t count,
    on_slave_read_discrete_inputs_f on_request
) {
    using namespace std::placeholders;
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    if (on_request) {
        auto f = initp::tools::function<mbs_slave_read_coils_f>::bind(RBIND(call_on_read_coils, _2, _3, _4, on_request));
        mbs_error_code_t errc = mbs_on_read_discrete_inputs(this->c_object_, address, count, std::get<0>(f), std::get<1>(f));
        if (errc) initp::tools::function_pointer<mbs_slave_read_coils_fp>::free(std::get<1>(f));
        return errc;
    } else return mbs_on_read_discrete_inputs(this->c_object_, address, count, NULL, 0);
}

mbs_error_code_t slave::on_read_holding_registers(
    uint16_t address,
    uint16_t count,
    on_slave_read_holding_registers_f on_request
) {
    using namespace std::placeholders;
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    if (on_request) {
        auto f = initp::tools::function<mbs_slave_read_holding_registers_f>::bind(RBIND(call_on_read_holding_registers, _2, _3, _4, on_request));
        mbs_error_code_t errc = mbs_on_read_holding_registers(this->c_object_, address, count, std::get<0>(f), std::get<1>(f));
        if (errc) initp::tools::function_pointer<mbs_slave_read_holding_registers_fp>::free(std::get<1>(f));
        return errc;
    } else return mbs_on_read_holding_registers(this->c_object_, address, count, NULL, 0);
}

mbs_error_code_t slave::on_read_input_registers(
    uint16_t address,
    uint16_t count,
    on_slave_read_input_registers_f on_request
) {
    using namespace std::placeholders;
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    if (on_request) {
        auto f = initp::tools::function<mbs_slave_read_holding_registers_f>::bind(RBIND(call_on_read_holding_registers, _2, _3, _4, on_request));
        mbs_error_code_t errc = mbs_on_read_input_registers(this->c_object_, address, count, std::get<0>(f), std::get<1>(f));
        if (errc) initp::tools::function_pointer<mbs_slave_read_holding_registers_fp>::free(std::get<1>(f));
        return errc;
    } else return mbs_on_read_input_registers(this->c_object_, address, count, NULL, 0);
}

mbs_error_code_t slave::on_write_multiple_coils(
    uint16_t address,
    uint16_t count,
    on_slave_write_multiple_coils_f on_request
) {
    using namespace std::placeholders;
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    if (on_request) {
        auto f = initp::tools::function<mbs_slave_write_multiple_coils_f>::bind(RBIND(call_on_write_multiple_coils, _2, _3, _4, on_request));
        mbs_error_code_t errc = mbs_on_write_multiple_coils(this->c_object_, address, count, std::get<0>(f), std::get<1>(f));
        if (errc) initp::tools::function_pointer<mbs_slave_write_multiple_coils_fp>::free(std::get<1>(f));
        return errc;
    } else return mbs_on_write_multiple_coils(this->c_object_, address, count, NULL, 0);
}

mbs_error_code_t slave::on_write_multiple_registers(
    uint16_t address,
    uint16_t count,
    on_slave_write_multiple_registers_f on_request
) {
    using namespace std::placeholders;
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    if (on_request) {
        auto f = initp::tools::function<mbs_slave_write_multiple_registers_f>::bind(RBIND(call_on_write_multiple_registers, _2, _3, _4, on_request));
        mbs_error_code_t errc = mbs_on_write_multiple_registers(this->c_object_, address, count, std::get<0>(f), std::get<1>(f));
        if (errc) initp::tools::function_pointer<mbs_slave_write_multiple_registers_fp>::free(std::get<1>(f));
        return errc;
    } else return mbs_on_write_multiple_registers(this->c_object_, address, count, NULL, 0);
}

    // Handler calls

mbs_byte_t slave::call_on_read_coils(uint16_t address, uint16_t count, uint8_t* coils, on_slave_read_coils_f on_request) {
    if (!on_request) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "modbus::slave::call_on_read_coils> No on_request callback");
        return MBS_ERRC_NULL_POINTER;
    }
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    return on_request(*this, address, count, coils);
}

mbs_byte_t slave::call_on_read_holding_registers(uint16_t address, uint16_t count, uint16_t* registers, on_slave_read_holding_registers_f on_request) {
    if (!on_request) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "modbus::slave::call_on_read_holding_registers> No on_request callback");
        return MBS_ERRC_NULL_POINTER;
    }
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    return on_request(*this, address, count, registers);
}

mbs_byte_t slave::call_on_write_multiple_coils(uint16_t address, uint16_t count, const uint8_t* coils, on_slave_write_multiple_coils_f on_request) {
    if (!on_request) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "modbus::slave::call_on_write_multiple_coils> No on_request callback");
        return MBS_ERRC_NULL_POINTER;
    }
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    return on_request(*this, address, count, coils);
}

mbs_byte_t slave::call_on_write_multiple_registers(uint16_t address, uint16_t count, const uint16_t* registers, on_slave_write_multiple_registers_f on_request) {
    if (!on_request) {
        mbs_debug_print(MBS_ERRC_NULL_POINTER, "modbus::slave::call_on_write_multiple_registers> No on_request callback");
        return MBS_ERRC_NULL_POINTER;
    }
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    return on_request(*this, address, count, registers);
}

    // Public properties

mbs_slave_tp slave::c_object(void) {
    return this->c_object_;
}

#ifdef MODBUS_ENABLE_THREAD_SAFETY
std::recursive_mutex& slave::mutex(void) {
    return this->mutex_;
}
#endif // MODBUS_ENABLE_THREAD_SAFETY

}
