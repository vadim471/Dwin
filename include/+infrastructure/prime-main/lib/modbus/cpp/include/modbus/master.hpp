#ifndef MODBUS_MASTER_HPP_INCLUDED
#define MODBUS_MASTER_HPP_INCLUDED

#include "config.hpp"
#include "callback.hpp"
#include "endpoint.hpp"
#include "device.hpp"

extern "C" {

#include <modbus/types.h>

}

#include <tuple>
#include <memory>
#include <vector>
#include <functional>

namespace modbus {

class master:
    public std::enable_shared_from_this<master> {
    friend class device;

private: // Private types
    typedef master self_type;

public: // Public types
    typedef std::shared_ptr<self_type> ptr;
    typedef std::unique_ptr<self_type> uptr;
    typedef std::weak_ptr<self_type> wptr;

public: // Construction
    master(void);
    master(const self_type&) = delete;
    virtual ~master(void);

public: // Public methods
    virtual void poll(mbs_time_t time);
    void dispose(void);
    void reinitialize(void);
    void endpoint(endpoint::uptr object);
    mbs_error_code_t expand(uint8_t size);
    std::tuple<device&, mbs_error_code_t> create_device(uint8_t address);
    std::tuple<device&, mbs_error_code_t> get_device(uint8_t address);

public: // Public properties
    mbs_master_tp c_object(void);
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::recursive_mutex& mutex(void);
    #endif // MODBUS_ENABLE_THREAD_SAFETY

private: // Private fields
    mbs_master_tp c_object_;
    endpoint::uptr endpoint_;
    #ifdef MODBUS_ENABLE_THREAD_SAFETY
    std::recursive_mutex mutex_;
    #endif // MODBUS_ENABLE_THREAD_SAFETY
    std::vector<device::uptr> devices_;
    device null_;
};

}

#endif // MODBUS_MASTER_HPP_INCLUDED
