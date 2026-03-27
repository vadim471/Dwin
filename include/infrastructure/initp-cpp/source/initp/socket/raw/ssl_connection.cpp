#include <initp/socket/raw/ssl_connection.hpp>

#include <initp/system/debug.hpp>
#include <initp/utils/macro.hpp>

#include <boost/asio/connect.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/write.hpp>
#include <boost/core/ignore_unused.hpp>
#include <boost/make_unique.hpp>

#include <thread>

namespace initp {
namespace socket {
namespace raw {

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
            sys_debug_print(SYSTEM_LEVEL_ERROR, "socket::raw::ssl_connection::initialize_ssl", "Failed to initialize SSL context. %s (%d)", ec.message().c_str(), ec.value());
            return ec;
        }
    } else {
        ssl.set_verify_mode(boost::asio::ssl::verify_none, ec);
        if (ec) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "socket::raw::ssl_connection::initialize_ssl", "Failed to set no verify mode, %s (%d)", ec.message().c_str(), ec.value());
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
    deadline_(ioc),
    stream_(boost::make_unique<stream_t>(this->context_, this->ssl_)),
    busy_(false),
    connected_(false)
{}

    // Public methods

system::error_code ssl_connection::connect(const std::string& host, const std::string& port, on_result_f on_result) {

    if (this->connected_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "socket::raw::ssl_connection::connect", "Already connected");
        return system::err::invalid_state;
    }

    if (this->busy_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "socket::raw::ssl_connection::connect", "Service is busy");
        return system::err::invalid_state;
    }

    // Set SNI Hostname (many hosts need this to handshake successfully)
    if (!SSL_set_tlsext_host_name(this->stream_->native_handle(), host.c_str())) {
        boost::system::error_code ec {
            static_cast<int>(::ERR_get_error()),
            boost::asio::error::get_ssl_category()
        };
        sys_debug_print(SYSTEM_LEVEL_ERROR, "socket::raw::ssl_connection::connect", "%s (%d)", ec.message().c_str(), ec.value());
        return system::err::internal_error;
    }

    this->host_ = host;
    this->port_ = port;

    this->busy_ = true;

    // Start the deadline actor. The connect and input actors will
    // update the deadline prior to each asynchronous operation.
    this->deadline_.async_wait(BIND(on_deadline, _1));

    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "socket::raw::ssl_connection::connect", "Trying to async resolve");
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
        sys_debug_print(SYSTEM_LEVEL_WARNING, "socket::raw::ssl_connection::disconnect", "Not connected");
        return system::err::invalid_state;
    }

    if (this->busy_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "socket::raw::ssl_connection::disconnect", "Service is busy");
        return system::err::invalid_state;
    }

    // Set a timeout on the operation
    this->deadline_.expires_after(std::chrono::seconds(15));

    this->busy_ = true;

    // Gracefully close the stream
    this->stream().async_shutdown(
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

system::error_code ssl_connection::write(const std::string& data, on_result_f on_result) {
    return this->write((const uint8_t*)data.c_str(), data.size(), on_result);
}

system::error_code ssl_connection::write(const std::vector<uint8_t>& data, on_result_f on_result) {
    return this->write(&data[0], data.size(), on_result);
}

system::error_code ssl_connection::write(const uint8_t* data, size_t size, on_result_f on_result) {

    if (!this->connected_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "socket::raw::ssl_connection::write", "Not connected");
        return system::err::invalid_state;
    }

    if (this->busy_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "socket::raw::ssl_connection::write", "Service is busy");
        return system::err::invalid_state;
    }

    // Set a timeout on the operation
    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "socket::raw::ssl_connection::write", "Trying to set write timer");
    this->deadline_.expires_after(std::chrono::seconds(5));

    // Send data to the remote host
    this->request_.resize(size);
    memcpy(&this->request_[0], data, size);
    boost::asio::async_write(
        this->stream(),
        boost::asio::buffer(this->request_, this->request_.size()),
        boost::asio::bind_executor(
            this->strand_,
            BIND(on_write, _1, _2, on_result)
        )
    );

    this->busy_ = true;
    return system::err::success;
}

    // Private callbacks

void ssl_connection::on_resolve(boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type results, on_result_f on_result) {

    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "socket::raw::ssl_connection::on_resolve", "%s (%d)", ec.message().c_str(), ec.value());
        if (on_result) on_result(ec);
        this->busy_ = false;
        return;
    }

    // Set a deadline for the connect operation
    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::ssl_connection::on_resolve", "Trying to set connection timer");
    this->deadline_.expires_after(std::chrono::seconds(5));

    // Make the connection on the IP address we get from a lookup
    boost::asio::async_connect(
        this->socket(),
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
        sys_debug_print(SYSTEM_LEVEL_ERROR, "socket::raw::ssl_connection::on_connect", "%s (%d)", ec.message().c_str(), ec.value());
        if (on_result) on_result(ec);
        this->busy_ = false;
        return;
    }

    // Set a deadline for the handshake operation
    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::ssl_connection::on_resolve", "Trying to set handshake timer");
    this->deadline_.expires_after(std::chrono::seconds(5));

    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "socket::raw::ssl_connection::on_connect", "Connected, call handshake");

    // Perform the SSL handshake
    this->stream().async_handshake(
        boost::asio::ssl::stream_base::client,
        boost::asio::bind_executor(
            this->strand_,
            BIND(on_handshake, _1, on_result)
        )
    );
}

void ssl_connection::on_handshake(boost::system::error_code ec, on_result_f on_result) {

    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "socket::raw::ssl_connection::on_handshake", "%s (%d)", ec.message().c_str(), ec.value());
        this->close_stream();
    } else {
        this->connected_ = true;
    }

    this->busy_ = false;
    if (on_result) on_result(ec);
}

void ssl_connection::on_write(boost::system::error_code ec, std::size_t bytes_transferred, on_result_f on_result) {

    boost::ignore_unused(bytes_transferred);

    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "socket::raw::ssl_connection::on_write", "%s (%d)", ec.message().c_str(), ec.value());
        return;
    }

    this->busy_ = false;
    if (on_result) on_result(ec);
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

void ssl_connection::on_deadline(boost::system::error_code ec) {
    if (ec) return;
    if (this->deadline_.expiry() <= boost::asio::steady_timer::clock_type::now()) {
        this->socket().close(ec);
        if (ec) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "socket::raw::ssl_connection::on_deadline", "Failed to close socket. %s (%d)", ec.message().c_str(), ec.value());
        }
        this->deadline_.expires_at(boost::asio::steady_timer::time_point::max());
    }
    this->deadline_.async_wait(BIND(on_deadline, _1));
}

void ssl_connection::close_stream(void) {
    boost::system::error_code ec;
    this->deadline_.cancel(ec);
    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "socket::raw::ssl_connection::close_stream", "Failed to cancel timer. %s (%d)", ec.message().c_str(), ec.value());
    }
    if (this->stream_) {
        this->socket().cancel(ec);
        if (ec) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "socket::raw::ssl_connection::close_stream", "Failed to cancel socket. %s (%d)", ec.message().c_str(), ec.value());
        }
        this->socket().close(ec);
        if (ec) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "socket::raw::ssl_connection::close_stream", "Failed to close socket. %s (%d)", ec.message().c_str(), ec.value());
        }
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

ssl_connection::socket_t& ssl_connection::socket(void) {
    return (socket_t&)this->stream_->lowest_layer();
}

ssl_connection::stream_t& ssl_connection::stream(void) {
    return *this->stream_.get();
}

}}}
