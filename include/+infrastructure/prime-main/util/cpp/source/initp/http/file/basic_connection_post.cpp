#include <initp/http/file/basic_connection.hpp>

#include <initp/system/debug.hpp>
#include <initp/system/macro.hpp>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>

#ifdef SYSTEM_TRACE_HTTP_REQUESTS
#include <sstream>
#endif // SYSTEM_TRACE_HTTP_REQUESTS

namespace initp {
namespace http {
namespace file {

using namespace std::placeholders;

    // GET method

system::error_code basic_connection::get(const std::string& source, const header_list_t& head, const std::string& destination, on_response_f on_response) {

    if (!this->connected_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::file::basic_connection::get", "Not connected");
        return system::err::invalid_state;
    }

    if (this->busy_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::file::basic_connection::get", "Service is busy");
        return system::err::invalid_state;
    }

    // Make target
    boost::filesystem::path path;
    if (!boost::algorithm::starts_with(source, "/") &&
        !boost::algorithm::starts_with(source, "\\"))
        path.append("/");
    path.append(source);
    path = path.lexically_normal();
    std::string target(path.string());
    std::replace(target.begin(), target.end(), '\\', '/');

    // Create directories
    path = boost::filesystem::path();
    path.append(destination);
    path = path.lexically_normal();
    if (!path.parent_path().string().empty() &&
        path.parent_path().compare(std::string(".")) != 0) {
        boost::system::error_code ec;
        boost::filesystem::create_directories(path.parent_path(), ec);
        if (ec) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "http::file::basic_connection::get", "Failed to create directories %s", path.parent_path().string().c_str());
            return ec;
        }
    }
    std::string filename(path.string());
    std::replace(filename.begin(), filename.end(), '\\', '/');

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
        sys_debug_print(SYSTEM_LEVEL_TRACE, "http::file::basic_connection::get", "%s", stream.str().c_str());
    }
    #endif // SYSTEM_TRACE_HTTP_REQUESTS

    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::file::basic_connection::get", "Trying to set write timer");
    boost::beast::get_lowest_layer(this->stream()).expires_after(std::chrono::seconds(5));

    // Send the HTTP request to the remote host
    boost::beast::http::async_write(
        this->stream(),
        this->request_,
        boost::asio::bind_executor(
            this->strand_,
            BIND(on_write, _1, _2, filename, on_response)
        )
    );

    this->busy_ = true;
    return system::err::success;
}

    // POST method

system::error_code basic_connection::post(const std::string& source, const header_list_t& head, const std::string& destination, on_response_f on_response) {

    if (!this->connected_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::file::basic_connection::post", "Not connected");
        return system::err::invalid_state;
    }

    if (this->busy_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::file::basic_connection::post", "Service is busy");
        return system::err::invalid_state;
    }

    // Make target
    boost::filesystem::path path;
    if (!boost::algorithm::starts_with(source, "/") &&
        !boost::algorithm::starts_with(source, "\\"))
        path.append("/");
    path.append(source);
    path = path.lexically_normal();
    std::string target(path.string());
    std::replace(target.begin(), target.end(), '\\', '/');

    // Create directories
    path = boost::filesystem::path();
    path.append(destination);
    path = path.lexically_normal();
    if (!path.parent_path().string().empty() &&
        path.parent_path().compare(std::string(".")) != 0) {
        boost::system::error_code ec;
        boost::filesystem::create_directories(path.parent_path(), ec);
        if (ec) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "http::file::basic_connection::post", "Failed to create directories %s", path.parent_path().string().c_str());
            return ec;
        }
    }
    std::string filename(path.string());
    std::replace(filename.begin(), filename.end(), '\\', '/');

    this->request_ = {};
    this->response_ = {};

    this->request_.method(boost::beast::http::verb::post);
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
        sys_debug_print(SYSTEM_LEVEL_TRACE, "http::file::basic_connection::post", "%s", stream.str().c_str());
    }
    #endif // SYSTEM_TRACE_HTTP_REQUESTS

    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::file::basic_connection::post", "Trying to set write timer");
    boost::beast::get_lowest_layer(this->stream()).expires_after(std::chrono::seconds(5));

    // Send the HTTP request to the remote host
    boost::beast::http::async_write(
        this->stream(),
        this->request_,
        boost::asio::bind_executor(
            this->strand_,
            BIND(on_write, _1, _2, filename, on_response)
        )
    );

    this->busy_ = true;
    return system::err::success;
}

}}}
