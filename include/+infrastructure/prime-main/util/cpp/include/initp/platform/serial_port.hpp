#ifndef INITPLUS_SERIAL_PORT_HPP_INCLUDED
#define INITPLUS_SERIAL_PORT_HPP_INCLUDED

extern "C" {

#include <initp/platform/serial_port.h>

}

namespace initp {
namespace system {

class serial_port {
private:
    typedef serial_port self_type;
public:
    serial_port(void):
        opened_(false) {
        sys_init_serial_port(&this->c_object_);
    }
    serial_port(const self_type&) = delete;
    virtual ~serial_port(void) {
        this->close();
    }
public:
    virtual bool open(const char* path, uint32_t baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity) {
        if (this->opened_) return true;
        this->opened_ = true;
        return sys_open_serial_port(&this->c_object_, path, baudrate, databits, stopbits, parity) > 0;
    }
    virtual bool reopen(void) {
        if (!this->opened_) {
            sys_reopen_serial_port(&this->c_object_);
            this->opened_ = true;
        }
        return true;
    }
    virtual void close(void) {
        if (this->opened_) {
            sys_close_serial_port(&this->c_object_);
            this->opened_ = false;
        }
    }
public:
    virtual size_t read(char* data, size_t size) {
        return sys_read_serial_port(&this->c_object_, data, size);
    }
    virtual size_t write(const char* data, size_t length) {
        return sys_write_serial_port(&this->c_object_, data, length);
    }
public:
    sys_serial_port_tp c_object(void) {
        return &this->c_object_;
    }
    bool opened(void) const {
        return this->opened_;
    }
protected:
    sys_serial_port_t c_object_;
    bool opened_;
};

}}

#endif // INITPLUS_SERIAL_PORT_HPP_INCLUDED
