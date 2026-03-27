#ifndef INITPLUS_HTTP_STRING_CONNECTION_HPP_INCLUDED
#define INITPLUS_HTTP_STRING_CONNECTION_HPP_INCLUDED

#include <initp/system/error_code.hpp>
#include <initp/http/header_list.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/io_context.hpp>

#include <memory>
#include <functional>
#include <vector>

namespace initp {
namespace http {
namespace string {

class connection {

private: // Private types
    typedef connection self_type;

public: // Public types
    typedef std::shared_ptr<self_type> ptr;
    typedef std::unique_ptr<self_type> uptr;
    typedef initp::http::header_list header_list_t;
    typedef std::function<void(system::error_code)> on_result_f;
    typedef std::function<void(system::error_code, boost::beast::http::status, const header_list_t&, const std::string&)> on_response_f;

public: // Construction
    connection(void) {}
    connection(const self_type&) = delete;
    virtual ~connection(void) = default;

public: // Public methods
    virtual system::error_code connect(const std::string&, const std::string&, on_result_f) = 0;
    virtual system::error_code get(const std::string&, const header_list_t&, on_response_f) = 0;
    virtual system::error_code post(const std::string&, const header_list_t&, const std::string&, on_response_f) = 0;
    virtual void close(void) = 0;

public: // Public properties
    virtual bool connected(void) const = 0;
    virtual bool busy(void) const = 0;
};

}}}

#endif // INITPLUS_HTTP_STRING_CONNECTION_HPP_INCLUDED
