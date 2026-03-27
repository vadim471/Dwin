#ifndef ITP_ENDPOINT_HPP_INCLUDED
#define ITP_ENDPOINT_HPP_INCLUDED

#include "config.hpp"

extern "C" {

#include <itp/endpoint.h>
#include <itp/memory.h>
#include <itp/debug.h>

}

#include <memory>
#include <iostream>

namespace itp {

itp_size_t endpoint_read(void*, itp_byte_t*, itp_size_t);
itp_size_t endpoint_write(void*, const itp_byte_t*, itp_size_t);

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
            0,
            (void*)this
        }
    {}
    endpoint(const endpoint&) = delete;
    virtual ~endpoint(void) = default;
public:
    virtual itp_size_t read(itp_byte_t* data, itp_size_t size) = 0;
    virtual itp_size_t write(const itp_byte_t* data, itp_size_t length) = 0;
public:
    bool use_checksum(void) const {
        return this->c_object_.use_checksum;
    }
    void use_checksum(bool value) {
        this->c_object_.use_checksum = value ? 1 : 0;
    }
    itp_endpoint_tp c_object(void) {
        return &this->c_object_;
    }
protected:
    itp_endpoint_t c_object_;
};

inline itp_size_t endpoint_read(void* object, itp_byte_t* data, itp_size_t length) {
    if (!object) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp::endpoint_read> User object is null");
        return 0;
    }
    return static_cast<endpoint*>(object)->read(data, length);
}

inline itp_size_t endpoint_write(void* object, const itp_byte_t* data, itp_size_t length) {
    if (!object) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp::endpoint_write> User object is null");
        return 0;
    }
    return static_cast<endpoint*>(object)->write(data, length);
}

/**
 * \brief Класс удалённого интерфейса приёма/передачи данных.
 */
class remote_endpoint:
    public itp::endpoint {
    typedef remote_endpoint self_type;
public:
    typedef std::unique_ptr<self_type> uptr;
public:
    remote_endpoint(itp_endpoint_tp ep):
        endpoint() {
        this->c_object_.object = ep->object;
        this->c_object_.use_checksum = ep->use_checksum;
        this->c_object_.read = ep->read;
        this->c_object_.write = ep->write;
        itp_free(ep);
    }
public:
    virtual itp_size_t read(itp_byte_t* data, itp_size_t size) {
        return this->c_object_.read(this->c_object_.object, data, size);
    }
    virtual itp_size_t write(const itp_byte_t* data, itp_size_t length) {
        return this->c_object_.write(this->c_object_.object, data, length);
    }
};

}

#endif // ITP_ENDPOINT_HPP_INCLUDED
