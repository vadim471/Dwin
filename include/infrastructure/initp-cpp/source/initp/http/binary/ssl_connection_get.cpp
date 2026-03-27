#include <initp/http/binary/ssl_connection.hpp>

#include <initp/system/debug.hpp>
#include <initp/utils/macro.hpp>

#ifdef SYSTEM_TRACE_HTTP_REQUESTS
#include <sstream>
#endif // SYSTEM_TRACE_HTTP_REQUESTS

namespace initp {
namespace http {
namespace binary {

using namespace std::placeholders;

    // GET method

system::error_code ssl_connection::get(const std::string& target, const header_list_t& head, const on_response_f& on_response) {

    if (!this->connected_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::binary::ssl_connection::get", "Not connected");
        return system::err::invalid_state;
    }

    if (this->busy_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::binary::ssl_connection::get", "Service is busy");
        return system::err::invalid_state;
    }

    this->empty_request_ = {};
    this->response_ = {};

    this->empty_request_.method(boost::beast::http::verb::get);
    this->empty_request_.target(target);
    this->empty_request_.version(11);
    this->empty_request_.set(boost::beast::http::field::host, this->host_);
    this->empty_request_.set(boost::beast::http::field::user_agent, this->agent_);
    for (auto const& it : head)
        this->empty_request_.set(it.first, it.second);
    this->empty_request_.prepare_payload();

    // Prepare response buffers
    this->response_data_.resize(this->max_response_size_);
    this->response_.body().data = &this->response_data_[0];
    this->response_.body().size = this->max_response_size_;
    this->response_.body().more = true;

    #ifdef SYSTEM_TRACE_HTTP_REQUESTS
    {
        std::stringstream stream;
        stream << "Request:" << std::endl << this->empty_request_;
        sys_debug_print(SYSTEM_LEVEL_TRACE, "http::binary::ssl_connection::get", "%s", stream.str().c_str());
    }
    #endif // SYSTEM_TRACE_HTTP_REQUESTS

    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::binary::ssl_connection::get", "Trying to set write timer");
    boost::beast::get_lowest_layer(this->stream()).expires_after(std::chrono::seconds(5));

    // Send the HTTP request to the remote host
    boost::beast::http::async_write(
        this->stream(),
        this->empty_request_,
        boost::asio::bind_executor(
            this->strand_,
            BIND(on_write, _1, _2, on_response)
        )
    );

    this->busy_ = true;
    return system::err::success;
}

}}}
