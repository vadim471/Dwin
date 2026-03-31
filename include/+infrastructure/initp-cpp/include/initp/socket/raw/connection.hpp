#ifndef INITPLUS_SOCKET_RAW_CONNECTION_HPP_INCLUDED
#define INITPLUS_SOCKET_RAW_CONNECTION_HPP_INCLUDED

#include <initp/system/error_code.hpp>

#include <boost/asio/io_context.hpp>

#include <memory>
#include <functional>
#include <vector>

namespace initp {
namespace socket {
namespace raw {

class connection {

private: // Private types
    typedef connection self_type;

public: // Public types
    typedef std::shared_ptr<self_type> ptr;
    typedef std::unique_ptr<self_type> uptr;
    typedef std::function<void(system::error_code)> on_result_f;

public: // Construction
    connection(void) {}
    connection(const self_type&) = delete;
    virtual ~connection(void) = default;

public: // Public methods
    virtual system::error_code connect(const std::string&, const std::string&, on_result_f) = 0;
    virtual system::error_code write(const std::string&, on_result_f) = 0;
    virtual system::error_code write(const std::vector<uint8_t>&, on_result_f) = 0;
    virtual system::error_code write(const uint8_t*, size_t, on_result_f) = 0;
    virtual void close(void) = 0;

public: // Public properties
    virtual bool connected(void) const = 0;
    virtual bool busy(void) const = 0;
};

}}}

#endif // INITPLUS_SOCKET_RAW_CONNECTION_HPP_INCLUDED
