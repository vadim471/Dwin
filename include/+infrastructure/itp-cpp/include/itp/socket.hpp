#ifndef ITP_SOCKET_HPP_INCLUDED
#define ITP_SOCKET_HPP_INCLUDED

#include <itp/endpoint.hpp>

#include <initp/platform/socket.hpp>

namespace itp {

class socket:
    public initp::system::socket,
    public itp::endpoint {
    friend class initp::system::socket::service;
private: // Private types
    typedef socket self_type;
public: // Public types
    typedef std::unique_ptr<self_type> uptr;
public: // Construction
    socket(void):
        initp::system::socket(),
        itp::endpoint() {}
    virtual ~socket(void) = default;
protected:
    socket(service::ptr obj):
        initp::system::socket(obj),
        itp::endpoint() {}
public: // Endpoint implementation
    virtual itp_size_t read(itp_byte_t* data, itp_size_t size) {
        return (itp_size_t)initp::system::socket::read((char*)data, (size_t)size);
    }
    virtual itp_size_t write(const itp_byte_t* data, itp_size_t length) {
        return (itp_size_t)initp::system::socket::write((const char*)data, (size_t)length);
    }
};

}

#endif // ITP_SOCKET_HPP_INCLUDED
