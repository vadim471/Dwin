#include <initp/http/ranged/ssl_connection.hpp>

#include <initp/system/debug.hpp>
#include <initp/system/macro.hpp>

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/predicate.hpp>

#ifdef SYSTEM_TRACE_HTTP_REQUESTS
#include <sstream>
#endif // SYSTEM_TRACE_HTTP_REQUESTS

namespace initp {
namespace http {
namespace ranged {

using namespace std::placeholders;

    // GET method

system::error_code ssl_connection::get(const std::string& target, const header_list_t& head, const std::string& filename, const on_progress_f& on_progress, const on_response_f& on_response) {

    if (!this->connected_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::ranged::ssl_connection::get", "Not connected");
        return system::err::invalid_state;
    }

    if (this->busy_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::ranged::ssl_connection::get", "Service is busy");
        return system::err::invalid_state;
    }

    std::string normalized_target = target;
    boost::replace_all(normalized_target, "\\", "/");
    if (!boost::starts_with(normalized_target, "/")) {
        normalized_target.insert(normalized_target.begin(), '/');
    }

    this->cache_errors_ = 0;
    this->cache_method_ = boost::beast::http::verb::get;
    this->cache_range_size_ = this->max_range_size_;
    this->cache_target_ = normalized_target;
    this->cache_filename_ = filename;
    this->cache_headers_ = head;

    this->request_ = {};

    this->request_.method(boost::beast::http::verb::head);
    this->request_.target(normalized_target);
    this->request_.version(11);
    this->request_.set(boost::beast::http::field::host, this->host_);
    this->request_.set(boost::beast::http::field::user_agent, this->agent_);
    this->request_.set(boost::beast::http::field::connection, "keep-alive");
    for (auto const& it : head)
        this->request_.set(it.first, it.second);
    this->request_.prepare_payload();

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
        sys_debug_print(SYSTEM_LEVEL_TRACE, "http::ranged::ssl_connection::get", "%s", stream.str().c_str());
    }
    #endif // SYSTEM_TRACE_HTTP_REQUESTS

    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::ranged::ssl_connection::get", "Trying to set write timer");
    boost::beast::get_lowest_layer(this->stream()).expires_after(std::chrono::seconds(5));

    // Send the HTTP request to the remote host
    boost::beast::http::async_write(
        this->stream(),
        this->request_,
        boost::asio::bind_executor(
            this->strand_,
            BIND(on_write_head, _1, _2, on_progress, on_response)
        )
    );

    this->busy_ = true;
    return system::err::success;
}

    // Range method

system::error_code ssl_connection::request_range(const std::string& range, const on_progress_f& on_progress, const on_response_f& on_response) {

    if (!this->connected_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::ranged::ssl_connection::request_range", "Not connected");
        return system::err::invalid_state;
    }

    this->cache_range_ = range;

    this->request_ = {};
    this->response_ = {};

    this->request_.method(this->cache_method_);
    this->request_.target(this->cache_target_);
    this->request_.version(11);
    this->request_.set(boost::beast::http::field::host, this->host_);
    this->request_.set(boost::beast::http::field::user_agent, this->agent_);
    this->request_.set(boost::beast::http::field::range, range);
    this->request_.set(boost::beast::http::field::connection, "keep-alive");
    for (auto const& it : this->cache_headers_)
        this->request_.set(it.first, it.second);
    this->request_.prepare_payload();

    // Prepare response buffers
    this->max_response_size_ = this->cache_range_size_ + 8192;
    this->response_data_.resize(this->max_response_size_);
    this->response_.body().data = &this->response_data_[0];
    this->response_.body().size = this->max_response_size_;
    this->response_.body().more = true;

    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::ranged::ssl_connection::request_range", "Trying to set write timer");
    boost::beast::get_lowest_layer(this->stream()).expires_after(std::chrono::seconds(3));

    // Send the HTTP request to the remote host
    boost::beast::http::async_write(
        this->stream(),
        this->request_,
        boost::asio::bind_executor(
            this->strand_,
            BIND(on_write, _1, _2, on_progress, on_response)
        )
    );

    this->busy_ = true;
    return system::err::success;
}

}}}
