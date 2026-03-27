#ifndef ITP_SERIAL_PORT_HPP_INCLUDED
#define ITP_SERIAL_PORT_HPP_INCLUDED

#include <itp/endpoint.hpp>

#include <initp/platform/serial_port.hpp>

#include <memory>

namespace itp {

class serial_port:
    public initp::system::serial_port,
    public itp::endpoint {
    using itp::endpoint::c_object;
private: // Private types
    typedef serial_port self_type;
public: // Public types
    typedef std::unique_ptr<self_type> uptr;
public: // Construction
    serial_port(void):
        initp::system::serial_port(),
        itp::endpoint() {}
    virtual ~serial_port(void) = default;
public: // Endpoint implementation
    virtual itp_size_t read(itp_byte_t* data, itp_size_t size) {
        return (itp_size_t)initp::system::serial_port::read((char*)data, (size_t)size);
    }
    virtual itp_size_t write(const itp_byte_t* data, itp_size_t length) {
        return (itp_size_t)initp::system::serial_port::write((const char*)data, (size_t)length);
    }
};

}

#endif // ITP_SERIAL_PORT_HPP_INCLUDED
