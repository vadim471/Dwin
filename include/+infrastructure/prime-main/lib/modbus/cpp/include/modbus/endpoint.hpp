#ifndef MODBUS_ENDPOINT_HPP_INCLUDED
#define MODBUS_ENDPOINT_HPP_INCLUDED

#include "config.hpp"

extern "C" {

#include <modbus/endpoint.h>
#include <modbus/debug.h>

}

#include <memory>

namespace modbus {

mbs_size_t endpoint_read(void*, mbs_byte_t*, mbs_size_t);
mbs_size_t endpoint_write(void*, const mbs_byte_t*, mbs_size_t);

/**
 * \brief Класс, позволяющий определить интерфейс приёма/передачи данных.
 *
 * Для нормального функционирования достаточно определить методы \c read/write.
 */
class endpoint {
    typedef endpoint self_type;
public:
    typedef std::unique_ptr<self_type> uptr;
public:
    endpoint(void):
        c_object_ {
            &endpoint_read,
            &endpoint_write,
            (void*)this
        }
    {}
    endpoint(const self_type&) = delete;
    virtual ~endpoint(void) = default;
public:
    virtual mbs_size_t read(mbs_byte_t* data, mbs_size_t size) = 0;
    virtual mbs_size_t write(const mbs_byte_t* data, mbs_size_t length) = 0;
public:
    mbs_endpoint_tp c_object(void) {
        return &this->c_object_;
    }
private:
    mbs_endpoint_t c_object_;
};

inline mbs_size_t endpoint_read(void* object, mbs_byte_t* data, mbs_size_t length) {
    if (!object) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "modbus::endpoint_read> User object is null");
        return 0;
    }
    return static_cast<endpoint*>(object)->read(data, length);
}

inline mbs_size_t endpoint_write(void* object, const mbs_byte_t* data, mbs_size_t length) {
    if (!object) {
        mbs_debug_print(MBS_ERRC_INTERNAL, "modbus::endpoint_write> User object is null");
        return 0;
    }
    return static_cast<endpoint*>(object)->write(data, length);
}

}

#endif // MODBUS_ENDPOINT_HPP_INCLUDED
