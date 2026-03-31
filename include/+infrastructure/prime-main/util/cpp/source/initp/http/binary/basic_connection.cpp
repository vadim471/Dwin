#include <initp/http/binary/basic_connection.hpp>

#include <initp/system/debug.hpp>
#include <initp/system/macro.hpp>

#include <boost/make_unique.hpp>
#include <boost/beast/version.hpp>

#include <thread>

#ifdef SYSTEM_TRACE_HTTP_REQUESTS
#include <sstream>
#endif // SYSTEM_TRACE_HTTP_REQUESTS

namespace initp {
namespace http {
namespace binary {

using namespace std::placeholders;

    // Construction

basic_connection::basic_connection(
    boost::asio::io_context& ioc
):
    connection(),
    context_(ioc),
    strand_(ioc),
    resolver_(ioc),
    stream_(boost::make_unique<boost::beast::tcp_stream>(this->context_)),
    busy_(false),
    connected_(false),
    max_response_size_(1024 * 1024),
    agent_(BOOST_BEAST_VERSION_STRING)
{}

basic_connection::basic_connection(
    boost::asio::io_context& ioc,
    const std::string& user_agent
):
    connection(),
    context_(ioc),
    strand_(ioc),
    resolver_(ioc),
    stream_(boost::make_unique<boost::beast::tcp_stream>(this->context_)),
    busy_(false),
    connected_(false),
    max_response_size_(1024 * 1024),
    agent_(user_agent)
{}

    // Public methods

system::error_code basic_connection::connect(const std::string& host, const std::string& port, const on_result_f& on_result) {

    if (this->connected_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::binary::basic_connection::connect", "Already connected");
        return system::err::invalid_state;
    }

    if (this->busy_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::binary::basic_connection::connect", "Service is busy");
        return system::err::invalid_state;
    }

    this->host_ = host;
    this->port_ = port;

    this->busy_ = true;
    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::binary::basic_connection::connect", "Trying to async resolve");

    this->resolver_.async_resolve(
        host,
        port,
        boost::asio::bind_executor(
            this->strand_,
            BIND(on_resolve, _1, _2, on_result)
        )
    );

    return system::err::success;
}

void basic_connection::close(void) {
    this->close_stream();
    if (this->connected_)
        this->connected_ = false;
    if (this->busy_) {
        this->busy_ = false;
    }
}

    // Private callbacks

void basic_connection::on_resolve(boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type results, const on_result_f& on_result) {

    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::binary::basic_connection::on_resolve", "%s (%d)", ec.message().c_str(), ec.value());
        this->busy_ = false;
        if (on_result) on_result(ec);
        return;
    }

    // Set a timeout on the operation
    // Bug: https://github.com/boostorg/beast/issues/1695
    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::basic_connection::on_resolve", "Trying to set connection timer");
    boost::beast::get_lowest_layer(this->stream()).expires_after(std::chrono::seconds(5));

    // Make the connection on the IP address we get from a lookup
    boost::beast::get_lowest_layer(this->stream()).async_connect(
        results,
        boost::asio::bind_executor(
            this->strand_,
            BIND(on_connect, _1, on_result)
        )
    );
}

void basic_connection::on_connect(boost::system::error_code ec, const on_result_f& on_result) {

    if (ec) {
        this->close_stream();
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::binary::basic_connection::on_connect", "%s (%d)", ec.message().c_str(), ec.value());
        this->busy_ = false;
        if (on_result) on_result(ec);
        return;
    } else {
        this->connected_ = true;
    }

    this->busy_ = false;
    if (on_result) on_result(ec);
}

void basic_connection::on_write(boost::system::error_code ec, std::size_t bytes_transferred, const on_response_f& on_response) {

    boost::ignore_unused(bytes_transferred);

    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::binary::basic_connection::on_write", "%s (%d)", ec.message().c_str(), ec.value());
        this->busy_ = false;
        if (on_response) {
            header_list_t null;
            on_response(ec, boost::beast::http::status::ok, null, nullptr, 0);
        }
        return;
    }

    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::basic_connection::on_write", "Data written, trying to set read timer");
    boost::beast::get_lowest_layer(this->stream()).expires_after(std::chrono::seconds(5));

    // Receive the HTTP response
    boost::beast::http::async_read(
        this->stream(),
        this->buffer_,
        this->response_,
        boost::asio::bind_executor(
            this->strand_,
            BIND(on_read, _1, _2, on_response)
        )
    );
}

void basic_connection::on_read(boost::system::error_code ec, std::size_t bytes_transferred, const on_response_f& on_response) {

    boost::ignore_unused(bytes_transferred);

    boost::beast::http::status status = this->response_.result();
    header_list_t headers;
    uint8_t* body = nullptr;
    size_t size = 0;

    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::binary::basic_connection::on_read", "%s (%d)", ec.message().c_str(), ec.value());
    } else {

        #ifdef SYSTEM_TRACE_HTTP_REQUESTS
        {
            std::stringstream stream;
            stream << "Response: " << std::endl;
            stream << this->response_.result_int() << " " << status << std::endl;
            for (auto const& it : this->response_)
                stream << it.name() << ": " << it.value()
                #if BOOST_VERSION <= 107700
                    .to_string()
                #endif // BOOST_VERSION
                << std::endl;
            stream << "Total data size: " << bytes_transferred << std::endl;
            sys_debug_print(SYSTEM_LEVEL_TRACE, "http::binary::basic_connection::on_read", "%s", stream.str().c_str());
        }
        #endif // SYSTEM_TRACE_HTTP_REQUESTS

        if (this->response_.body().size > this->max_response_size_) {
            sys_debug_print(SYSTEM_LEVEL_FATAL, "http::binary::basic_connection::on_read", "Segmentation fault");
            this->busy_ = false;
            if (on_response) {
                header_list_t null;
                on_response(system::err::internal_error, status, null, nullptr, 0);
            }
            return;
        } else {
            size = this->max_response_size_ - this->response_.body().size;
            if (size) body = &this->response_data_[0];
        }

        for (auto const& it : this->response_) {
            headers[it.name()] = it.value()
                #if BOOST_VERSION <= 107700
                    .to_string()
                #endif // BOOST_VERSION
            ;
        }
        if (status == boost::beast::http::status::length_required) {
            status = boost::beast::http::status::ok;
        }
    }


    this->busy_ = false;
    if (on_response) on_response(ec, status, headers, body, size);
}

void basic_connection::close_stream(void) {
    if (this->stream_) {
        boost::beast::get_lowest_layer(this->stream()).cancel();
        boost::beast::get_lowest_layer(this->stream()).close();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        this->stream_.reset();
    }
    this->stream_ = boost::make_unique<boost::beast::tcp_stream>(this->context_);
}

    // Public properties

bool basic_connection::connected(void) const {
    return this->connected_;
}

bool basic_connection::busy(void) const {
    return this->busy_;
}

    // Private properties

boost::beast::tcp_stream& basic_connection::stream(void) {
    return *this->stream_.get();
}

}}}
