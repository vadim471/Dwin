#ifndef ITP_NAMED_PIPE_HPP_INCLUDED
#define ITP_NAMED_PIPE_HPP_INCLUDED

#include <itp/endpoint.hpp>

#include <initp/platform/named_pipe.hpp>

#include <memory>

namespace itp {

class named_pipe:
    public initp::system::named_pipe,
    public itp::endpoint {
    using itp::endpoint::c_object;
private: // Private types
    typedef named_pipe self_type;
public: // Public types
    typedef std::unique_ptr<self_type> uptr;
public: // Construction
    named_pipe(const char* path):
        initp::system::named_pipe(path),
        itp::endpoint() {}
    named_pipe(const std::string& path):
        initp::system::named_pipe(path),
        itp::endpoint() {}
    virtual ~named_pipe(void) = default;
public: // Endpoint implementation
    virtual itp_size_t read(itp_byte_t* data, itp_size_t size) {
        return (itp_size_t)initp::system::named_pipe::read((char*)data, (size_t)size);
    }
    virtual itp_size_t write(const itp_byte_t* data, itp_size_t length) {
        return (itp_size_t)initp::system::named_pipe::write((const char*)data, (size_t)length);
    }
};

}

#endif // ITP_NAMED_PIPE_HPP_INCLUDED
