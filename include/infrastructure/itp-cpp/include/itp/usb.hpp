#ifndef ITP_USB_HPP_INCLUDED
#define ITP_USB_HPP_INCLUDED

#include <itp/endpoint.hpp>

#include <initp/platform/usb.hpp>

#include <memory>

namespace itp {

class usb:
    public initp::system::usb_context,
    public itp::endpoint {
    using itp::endpoint::c_object;
private: // Private types
    typedef usb self_type;
public: // Public types
    typedef std::unique_ptr<self_type> uptr;
public: // Construction
    usb(void):
        initp::system::usb_context(),
        itp::endpoint() {}
    virtual ~usb(void) = default;
public: // Endpoint implementation
    virtual itp_size_t read(itp_byte_t* data, itp_size_t size) {
        return (itp_size_t)initp::system::usb_context::read((char*)data, (size_t)size);
    }
    virtual itp_size_t write(const itp_byte_t* data, itp_size_t length) {
        return (itp_size_t)initp::system::usb_context::write((const char*)data, (size_t)length);
    }
};

}

#endif // ITP_USB_HPP_INCLUDED
