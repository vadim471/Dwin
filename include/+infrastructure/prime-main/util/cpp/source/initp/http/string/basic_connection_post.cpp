#include <initp/http/string/basic_connection.hpp>

#include <initp/system/debug.hpp>
#include <initp/system/macro.hpp>

#ifdef SYSTEM_TRACE_HTTP_REQUESTS
#include <sstream>
#endif // SYSTEM_TRACE_HTTP_REQUESTS

namespace initp {
namespace http {
namespace string {

using namespace std::placeholders;

    // GET method

system::error_code basic_connection::get(const std::string& target, const header_list_t& head, on_response_f on_response) {

    if (!this->connected_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::string::basic_connection::get", "Not connected");
        return system::err::invalid_state;
    }

    if (this->busy_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::string::basic_connection::get", "Service is busy");
        return system::err::invalid_state;
    }

    this->request_ = {};
    this->response_ = {};

    this->request_.method(boost::beast::http::verb::get);
    this->request_.target(target);
    this->request_.version(11);
    this->request_.set(boost::beast::http::field::host, this->host_);
    this->request_.set(boost::beast::http::field::user_agent, this->agent_);
    for (auto const& it : head)
        this->request_.set(it.first, it.second);
    this->request_.prepare_payload();

    #ifdef SYSTEM_TRACE_HTTP_REQUESTS
    {
        std::stringstream stream;
        stream << "Request:" << std::endl << this->request_;
        sys_debug_print(SYSTEM_LEVEL_TRACE, "http::string::basic_connection::get", "%s", stream.str().c_str());
    }
    #endif // SYSTEM_TRACE_HTTP_REQUESTS

    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::string::basic_connection::get", "Trying to set write timer");
    boost::beast::get_lowest_layer(this->stream()).expires_after(std::chrono::seconds(5));

    // Send the HTTP request to the remote host
    boost::beast::http::async_write(
        this->stream(),
        this->request_,
        boost::asio::bind_executor(
            this->strand_,
            BIND(on_write, _1, _2, on_response)
        )
    );

    this->busy_ = true;
    return system::err::success;
}

    // POST method

system::error_code basic_connection::post(const std::string& target, const header_list_t& head, const std::string& body, on_response_f on_response) {

    if (!this->connected_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::string::basic_connection::post", "Not connected");
        return system::err::invalid_state;
    }

    if (this->busy_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::string::basic_connection::post", "Service is busy");
        return system::err::invalid_state;
    }

    this->request_ = {};
    this->response_ = {};

    this->request_.method(boost::beast::http::verb::post);
    this->request_.target(target);
    this->request_.version(11);
    this->request_.set(boost::beast::http::field::host, this->host_);
    this->request_.set(boost::beast::http::field::user_agent, this->agent_);
    for (auto const& it : head)
        this->request_.set(it.first, it.second);
    this->request_.body() = body;
    this->request_.prepare_payload();

    #ifdef SYSTEM_TRACE_HTTP_REQUESTS
    {
        std::stringstream stream;
        stream << "Request:" << std::endl << this->request_;
        sys_debug_print(SYSTEM_LEVEL_TRACE, "http::string::basic_connection::post", "%s", stream.str().c_str());
    }
    #endif // SYSTEM_TRACE_HTTP_REQUESTS

    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::string::basic_connection::post", "Trying to set write timer");
    boost::beast::get_lowest_layer(this->stream()).expires_after(std::chrono::seconds(5));

    // Send the HTTP request to the remote host
    boost::beast::http::async_write(
        this->stream(),
        this->request_,
        boost::asio::bind_executor(
            this->strand_,
            BIND(on_write, _1, _2, on_response)
        )
    );

    this->busy_ = true;
    return system::err::success;
}

}}}
