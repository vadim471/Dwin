#ifndef INITPLUS_HTTP_BINARY_CONNECTION_HPP_INCLUDED
#define INITPLUS_HTTP_BINARY_CONNECTION_HPP_INCLUDED

#include <initp/system/error_code.hpp>
#include <initp/http/header_list.hpp>

#include <boost/beast/http/status.hpp>

#include <memory>
#include <functional>
#include <vector>
#include <cstdint>
#include <cstddef>

namespace initp {
namespace http {
namespace binary {

class connection {

private: // Private types
    typedef connection self_type;

public: // Public types
    typedef std::shared_ptr<self_type> ptr;
    typedef initp::http::header_list header_list_t;
    typedef std::function<void(system::error_code)> on_result_f;
    typedef std::function<void(system::error_code, boost::beast::http::status, const header_list_t&, const uint8_t*, size_t)> on_response_f;

public: // Construction
    connection(void) {}
    connection(const self_type&) = delete;
    virtual ~connection(void) = default;

public: // Public methods
    virtual system::error_code connect(const std::string&, const std::string&, const on_result_f&) = 0;
    virtual system::error_code get(const std::string&, const header_list_t&, const on_response_f&) = 0;
    virtual system::error_code post(const std::string&, const header_list_t&, const std::vector<uint8_t>&, const on_response_f&) = 0;
    virtual system::error_code post(const std::string&, const header_list_t&, const uint8_t*, size_t, const on_response_f&) = 0;
    virtual void close(void) = 0;

public: // Public properties
    virtual bool connected(void) const = 0;
    virtual bool busy(void) const = 0;
};

}}}

#endif // INITPLUS_HTTP_CONNECTION_HPP_INCLUDED
