#include <modbus/master.hpp>

extern "C" {

#include <modbus/master.h>
#include <modbus/memory.h>

}

#include <initp/system/macro.hpp>

namespace modbus {

    // Construction

master::master(void):
    c_object_(NULL),
    null_(*this, NULL) {
    this->c_object_ = (mbs_master_tp)mbs_malloc(sizeof(mbs_master_t), "modbus::master:mbs_master");
    if (this->c_object_) {
        mbs_init_master_rtu(this->c_object_, NULL);
    }
    //std::cout << "modbus::master::master();" << std::endl;
}

master::~master(void) {
    if (this->c_object_) {
        mbs_free_master(this->c_object_);
    }
    //std::cout << "modbus::master::~master();" << std::endl;
}

    // Public methods

void master::poll(mbs_time_t time) {
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    mbs_poll_master(this->c_object_, time);
}

void master::dispose(void) {
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    mbs_dispose_master(this->c_object_);
    this->endpoint_.reset();
}

void master::reinitialize(void) {
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    mbs_init_master_rtu(this->c_object_, NULL);
}

void master::endpoint(endpoint::uptr object) {
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

mbs_error_code_t master::expand(uint8_t size) {
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    return mbs_expand_master(this->c_object_, size);
}

std::tuple<device&, mbs_error_code_t> master::create_device(uint8_t address) {
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    mbs_device_tp object = NULL;
    mbs_error_code_t errc = mbs_create_device(this->c_object_, address, &object);
    if (errc) return std::make_tuple(std::ref(this->null_), errc);
    this->devices_.emplace_back(new device(*this, object));
    return std::make_tuple(std::ref(*this->devices_.back().get()), errc);
}

std::tuple<device&, mbs_error_code_t> master::get_device(uint8_t address) {
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::lock_guard<std::recursive_mutex> lock(this->mutex_);
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    mbs_device_tp object = mbs_get_device(this->c_object_, address);
    if (!object) {
        return std::make_tuple(std::ref(this->null_), MBS_ERRC_WRONG_ADDRESS);
    }
    for (device::uptr& d : this->devices_) {
        if (d->c_object() == object) {
            return std::make_tuple(std::ref(*d.get()), MBS_ERRC_NONE);
        }
    }
    return std::make_tuple(std::ref(this->null_), MBS_ERRC_INTERNAL);
}

    // Public properties

mbs_master_tp master::c_object(void) {
    return this->c_object_;
}

#ifdef MODBUS_ENABLE_THREAD_SAFETY
std::recursive_mutex& master::mutex(void) {
    return this->mutex_;
}
#endif // MODBUS_ENABLE_THREAD_SAFETY

}
