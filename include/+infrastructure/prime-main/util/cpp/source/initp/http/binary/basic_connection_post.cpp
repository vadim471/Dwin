#include <initp/http/binary/basic_connection.hpp>

#include <initp/system/debug.hpp>
#include <initp/system/macro.hpp>

#include <cstring>

#ifdef SYSTEM_TRACE_HTTP_REQUESTS
#include <sstream>
#endif // SYSTEM_TRACE_HTTP_REQUESTS

namespace initp {
namespace http {
namespace binary {

using namespace std::placeholders;

    // GET method

system::error_code basic_connection::get(const std::string& target, const header_list_t& head, const on_response_f& on_response) {

    if (!this->connected_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::binary::basic_connection::get", "Not connected");
        return system::err::invalid_state;
    }

    if (this->busy_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::binary::basic_connection::get", "Service is busy");
        return system::err::invalid_state;
    }

    this->empty_request_ = {};
    this->response_ = {};

    this->empty_request_.method(boost::beast::http::verb::get);
    this->empty_request_.target(target);
    this->empty_request_.version(11);
    this->empty_request_.set(boost::beast::http::field::host, this->host_);
    this->empty_request_.set(boost::beast::http::field::user_agent, this->agent_);
    for (auto const& it : head.list())
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
        sys_debug_print(SYSTEM_LEVEL_TRACE, "http::binary::basic_connection::get", "%s", stream.str().c_str());
    }
    #endif // SYSTEM_TRACE_HTTP_REQUESTS

    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::binary::basic_connection::get", "Trying to set write timer");
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

    // POST method

system::error_code basic_connection::post(const std::string& target, const header_list_t& head, const std::vector<uint8_t>& body, const on_response_f& on_response) {

    if (!this->connected_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::binary::basic_connection::post", "Not connected");
        return system::err::invalid_state;
    }

    if (this->busy_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::binary::basic_connection::post", "Service is busy");
        return system::err::invalid_state;
    }

    this->request_ = {};
    this->response_ = {};

    this->request_.method(boost::beast::http::verb::post);
    this->request_.target(target);
    this->request_.version(11);
    this->request_.set(boost::beast::http::field::host, this->host_);
    this->request_.set(boost::beast::http::field::user_agent, this->agent_);
    for (auto const& it : head.list())
        this->request_.set(it.first, it.second);

    if (size_t size = body.size()) {
        this->request_data_.resize(size);
        memcpy(&this->request_data_[0], &body[0], size);
        this->request_.body().data = &this->request_data_[0];
        this->request_.body().size = this->request_data_.size();
    }
    this->request_.body().more = false;
    this->request_.prepare_payload();

    // Prepare response buffers
    this->response_data_.resize(this->max_response_size_);
    this->response_.body().data = &this->response_data_[0];
    this->response_.body().size = this->max_response_size_;
    this->response_.body().more = true;

    #ifdef SYSTEM_TRACE_HTTP_REQUESTS
    {
        std::stringstream stream;
        stream << "Request:" << std::endl;
        stream << this->request_.method() << " ";
        stream << this->request_.target() << " ";
        stream << "HTTP/" << ((double)this->request_.version() / 10.0) << std::endl;
        for (auto const& it : this->request_)
            stream << it.name() << ": " << it.value()
            #if BOOST_VERSION <= 107700
                .to_string()
            #endif // BOOST_VERSION
            << std::endl;
        sys_debug_print(SYSTEM_LEVEL_TRACE, "http::binary::basic_connection::post", "%s", stream.str().c_str());
    }
    #endif // SYSTEM_TRACE_HTTP_REQUESTS

    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::binary::basic_connection::post", "Trying to set write timer");
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

system::error_code basic_connection::post(const std::string& target, const header_list_t& head, const uint8_t* body, size_t size, const on_response_f& on_response) {

    if (!this->connected_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::binary::basic_connection::post", "Not connected");
        return system::err::invalid_state;
    }

    if (this->busy_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::binary::basic_connection::post", "Service is busy");
        return system::err::invalid_state;
    }

    this->request_ = {};
    this->response_ = {};

    this->request_.method(boost::beast::http::verb::post);
    this->request_.target(target);
    this->request_.version(11);
    this->request_.set(boost::beast::http::field::host, this->host_);
    this->request_.set(boost::beast::http::field::user_agent, this->agent_);
    for (auto const& it : head.list())
        this->request_.set(it.first, it.second);

    if (size) {
        this->request_data_.resize(size);
        memcpy(&this->request_data_[0], body, size);
        this->request_.body().data = &this->request_data_[0];
        this->request_.body().size = this->request_data_.size();
    }
    this->request_.body().more = false;
    this->request_.prepare_payload();

    // Prepare response buffers
    this->response_data_.resize(this->max_response_size_);
    this->response_.body().data = &this->response_data_[0];
    this->response_.body().size = this->max_response_size_;
    this->response_.body().more = true;

    #ifdef SYSTEM_TRACE_HTTP_REQUESTS
    {
        std::stringstream stream;
        stream << "Request:" << std::endl;
        stream << this->request_.method() << " ";
        stream << this->request_.target() << " ";
        stream << "HTTP/" << ((double)this->request_.version() / 10.0) << std::endl;
        for (auto const& it : this->request_)
            stream << it.name() << ": " << it.value()
            #if BOOST_VERSION <= 107700
                .to_string()
            #endif // BOOST_VERSION
            << std::endl;
        sys_debug_print(SYSTEM_LEVEL_TRACE, "http::binary::basic_connection::post", "%s", stream.str().c_str());
    }
    #endif // SYSTEM_TRACE_HTTP_REQUESTS

    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::binary::basic_connection::post", "Trying to set write timer");
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
