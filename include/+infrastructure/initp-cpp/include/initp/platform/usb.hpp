#ifndef INITPLUS_USB_HPP_INCLUDED
#define INITPLUS_USB_HPP_INCLUDED

extern "C" {

#include <initp/platform/usb.h>

}

namespace initp {
namespace system {

class usb_context {
private:
    typedef usb_context self_type;
public:
    usb_context(void) {
        sys_init_usb_context(&this->c_object_);
    }
    usb_context(const self_type&) = delete;
    virtual ~usb_context(void) {
        this->close();
    }
public:
    virtual bool connect(uint16_t vid, uint16_t pid, uint8_t ifc, uint8_t ep_in, uint8_t ep_out) {
        if (sys_connect_usb_device(&this->c_object_, vid, pid, ifc, ep_in, ep_out)) {
            return true;
        } else return false;
    }
    virtual void close(void) {
        sys_close_usb_context(&this->c_object_);
    }
public:
    virtual size_t read(char* data, size_t size) {
        return sys_read_usb_device(&this->c_object_, data, size);
    }
    virtual size_t write(const char* data, size_t length) {
        return sys_write_usb_device(&this->c_object_, data, length);
    }
public:
    sys_usb_context_tp c_object(void) {
        return &this->c_object_;
    }
protected:
    sys_usb_context_t c_object_;
};

}}

#endif // INITPLUS_USB_HPP_INCLUDED
