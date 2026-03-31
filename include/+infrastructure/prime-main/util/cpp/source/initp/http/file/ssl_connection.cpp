#include <initp/http/file/ssl_connection.hpp>

#include <initp/system/debug.hpp>
#include <initp/system/macro.hpp>

#include <boost/make_unique.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/beast/version.hpp>

#include <thread>

#ifdef SYSTEM_TRACE_HTTP_REQUESTS
#include <sstream>
#endif // SYSTEM_TRACE_HTTP_REQUESTS

namespace initp {
namespace http {
namespace file {

using namespace std::placeholders;

    // Static methods

system::error_code ssl_connection::initialize_ssl(boost::asio::ssl::context& ssl, const std::string& cacert, const std::string& clcert, const std::string& key) {

    boost::system::error_code ec;

    if (!cacert.empty() && !clcert.empty() && !key.empty()) {
        ssl.load_verify_file(cacert, ec);
        if (!ec) ssl.use_certificate_chain_file(clcert, ec);
        //if (!ec) ssl.use_certificate_file(clcert, boost::asio::ssl::context::pem, ec);
        if (!ec) ssl.use_private_key_file(key, boost::asio::ssl::context::pem, ec);
        if (!ec) ssl.set_verify_mode(boost::asio::ssl::verify_peer | boost::asio::ssl::verify_fail_if_no_peer_cert, ec);
        if (ec) {
            {
                boost::system::error_code ec;
                ssl.set_verify_mode(boost::asio::ssl::verify_none, ec);
            }
            sys_debug_print(SYSTEM_LEVEL_ERROR, "http::file::ssl_connection::initialize_ssl", "Failed to initialize SSL context. %s (%d)", ec.message().c_str(), ec.value());
            return ec;
        }
    } else {
        ssl.set_verify_mode(boost::asio::ssl::verify_none, ec);
        if (ec) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "http::file::ssl_connection::initialize_ssl", "Failed to set no verify mode, %s (%d)", ec.message().c_str(), ec.value());
            return ec;
        }
    }
    return system::err::success;
}

    // Construction

ssl_connection::ssl_connection(
    boost::asio::io_context& ioc,
    boost::asio::ssl::context& ssl
):
    connection(),
    context_(ioc),
    ssl_(ssl),
    strand_(ioc),
    resolver_(ioc),
    stream_(boost::make_unique<stream_t>(this->context_, this->ssl_)),
    busy_(false),
    connected_(false),
    agent_(BOOST_BEAST_VERSION_STRING)
{}

ssl_connection::ssl_connection(
    boost::asio::io_context& ioc,
    boost::asio::ssl::context& ssl,
    const std::string& user_agent
):
    connection(),
    context_(ioc),
    ssl_(ssl),
    strand_(ioc),
    resolver_(ioc),
    stream_(boost::make_unique<stream_t>(this->context_, this->ssl_)),
    busy_(false),
    connected_(false),
    agent_(user_agent)
{}

    // Public methods

system::error_code ssl_connection::connect(const std::string& host, const std::string& port, on_result_f on_result) {

    if (this->connected_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::file::ssl_connection::connect", "Already connected");
        return system::err::invalid_state;
    }

    if (this->busy_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::file::ssl_connection::connect", "Service is busy");
        return system::err::invalid_state;
    }

    this->host_ = host;
    this->port_ = port;

    // Set SNI Hostname (many hosts need this to handshake successfully)
    if (!SSL_set_tlsext_host_name(this->stream_->native_handle(), host.c_str())) {
        boost::beast::error_code ec {
            static_cast<int>(::ERR_get_error()),
            boost::asio::error::get_ssl_category()
        };
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::file::ssl_connection::connect", "%s (%d)", ec.message().c_str(), ec.value());
        return system::err::internal_error;
    }

    this->busy_ = true;
    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::file::ssl_connection::connect", "Trying to async resolve");

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

system::error_code ssl_connection::disconnect(on_result_f on_result) {

    if (!this->connected_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::file::ssl_connection::disconnect", "Not connected");
        return system::err::invalid_state;
    }

    if (this->busy_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::file::ssl_connection::disconnect", "Service is busy");
        return system::err::invalid_state;
    }

    // Set a timeout on the operation
    //boost::beast::get_lowest_layer(this->stream()).expires_after(std::chrono::seconds(15));

    this->busy_ = true;

    // Gracefully close the stream
    this->stream_->async_shutdown(
        boost::asio::bind_executor(
            this->strand_,
            BIND(on_shutdown, _1, on_result)
        )
    );
    return system::err::success;
}

void ssl_connection::close(void) {
    this->close_stream();
    if (this->connected_)
        this->connected_ = false;
    if (this->busy_) {
        this->busy_ = false;
    }
}

    // Private callbacks

void ssl_connection::on_resolve(boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type results, on_result_f on_result) {

    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::file::ssl_connection::on_resolve", "%s (%d)", ec.message().c_str(), ec.value());
        if (on_result) on_result(ec);
        this->busy_ = false;
        return;
    }

    // Set a timeout on the operation
    // Bug: https://github.com/boostorg/beast/issues/1695
    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::file::ssl_connection::on_resolve", "Trying to set connection timer");
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

void ssl_connection::on_connect(boost::system::error_code ec, on_result_f on_result) {

    if (ec) {
        this->close_stream();
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::file::ssl_connection::on_connect", "%s (%d)", ec.message().c_str(), ec.value());
        if (on_result) on_result(ec);
        this->busy_ = false;
        return;
    }

    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::file::ssl_connection::on_connect", "Connected, call handshake");

    // Perform the SSL handshake
    this->stream_->async_handshake(
        boost::asio::ssl::stream_base::client,
        boost::asio::bind_executor(
            this->strand_,
            BIND(on_handshake, _1, on_result)
        )
    );
}

void ssl_connection::on_handshake(boost::system::error_code ec, on_result_f on_result) {

    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::file::ssl_connection::on_handshake", "%s (%d)", ec.message().c_str(), ec.value());
        this->close_stream();
    } else {
        this->connected_ = true;
    }

    this->busy_ = false;
    if (on_result) on_result(ec);

    /*if (ec) {
        this->close_stream();
    }*/
}

void ssl_connection::on_write(boost::system::error_code ec, std::size_t bytes_transferred, const std::string& filename, on_response_f on_response) {

    boost::ignore_unused(bytes_transferred);

    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::file::ssl_connection::on_write", "%s (%d)", ec.message().c_str(), ec.value());
        if (on_response) {
            header_list_t null;
            on_response(ec, boost::beast::http::status::ok, null, 0);
        }
        this->busy_ = false;
        return;
    }

    this->response_.body().open(filename.c_str(), boost::beast::file_mode::write, ec);
    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::file::ssl_connection::on_write", "%s (%d)", ec.message().c_str(), ec.value());
        if (on_response) {
            header_list_t null;
            on_response(ec, boost::beast::http::status::ok, null, 0);
        }
        this->busy_ = false;
        return;
    }

    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::file::ssl_connection::on_write", "Data written, trying to set read timer");
    boost::beast::get_lowest_layer(this->stream()).expires_after(std::chrono::seconds(10));

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

void ssl_connection::on_read(boost::system::error_code ec, std::size_t bytes_transferred, on_response_f on_response) {

    boost::ignore_unused(bytes_transferred);
    boost::beast::http::status status = this->response_.result();
    size_t size = this->response_.body().size();

    if (!size && status == boost::beast::http::status::ok) {
        auto it = this->response_.find(boost::beast::http::field::content_length);
        if (it != this->response_.end()) {
            try {
                size = boost::lexical_cast<size_t>(this->response_[boost::beast::http::field::content_length]);
            } catch (const boost::bad_lexical_cast&) {}
        }
    }
    this->response_.body().close();

    header_list_t headers;
    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::file::ssl_connection::on_read", "%s (%d)", ec.message().c_str(), ec.value());
    } else {

        #ifdef SYSTEM_TRACE_HTTP_REQUESTS
        {
            std::stringstream stream;
            stream << "Response: " << this->response_.result() << std::endl;
            for (auto const& it : this->response_)
                stream << it.name() << ": " << it.value() << std::endl;
            stream << "Total data size: " << bytes_transferred << std::endl;
            sys_debug_print(SYSTEM_LEVEL_TRACE, "http::file::ssl_connection::on_read", "%s", stream.str().c_str());
        }
        #endif // SYSTEM_TRACE_HTTP_REQUESTS

        for (auto const& it : this->response_) {
            headers[it.name()] = it.value()
                #if BOOST_VERSION <= 107700
                    .to_string()
                #endif // BOOST_VERSION
            ;
        }
    }


    this->busy_ = false;
    if (on_response) on_response(ec, status, headers, size);
}

void ssl_connection::on_shutdown(boost::system::error_code ec, on_result_f on_result) {
    this->connected_ = false;
    if (ec) {
        if (ec == boost::asio::error::eof) {
            ec = {};
        } else {
            this->close_stream();
        }
    }
    this->busy_ = false;
    if (on_result) on_result(ec);
}

void ssl_connection::close_stream(void) {
    if (this->stream_) {
        boost::beast::get_lowest_layer(this->stream()).cancel();
        boost::beast::get_lowest_layer(this->stream()).close();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        this->stream_.reset();
    }
    this->stream_ = boost::make_unique<stream_t>(this->context_, this->ssl_);
}

    // Public properties

bool ssl_connection::connected(void) const {
    return this->connected_;
}

bool ssl_connection::busy(void) const {
    return this->busy_;
}

    // Private properties

ssl_connection::stream_t& ssl_connection::stream(void) {
    return *this->stream_.get();
}

}}}
