#ifndef SERIAL_HPP_INCLUDED
#define SERIAL_HPP_INCLUDED

#include <modbus/endpoint.hpp>
#include <initp/platform/serial_port.hpp>

class serial:
    public initp::system::serial_port,
    public modbus::endpoint {
    using modbus::endpoint::c_object;
private:
    typedef serial self_type;
public:
    typedef std::unique_ptr<self_type> uptr;
public:
    serial(void):
        serial_port(),
        endpoint() {
    }
    serial(const self_type&) = delete;
    virtual ~serial(void) = default;
public:
    virtual mbs_size_t read(mbs_byte_t* data, mbs_size_t size) {
        return (mbs_size_t)serial_port::read((char*)data, (size_t)size);
    }
    virtual mbs_size_t write(const mbs_byte_t* data, mbs_size_t length) {
        return (mbs_size_t)serial_port::write((const char*)data, (size_t)length);
    }
};

#endif // SERIAL_HPP_INCLUDED
