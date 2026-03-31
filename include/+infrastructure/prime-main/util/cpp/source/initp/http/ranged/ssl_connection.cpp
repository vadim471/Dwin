#include <initp/http/ranged/ssl_connection.hpp>

#include <initp/system/debug.hpp>
#include <initp/system/macro.hpp>
#include <initp/tools/string.hpp>

#include <boost/make_unique.hpp>
#include <boost/beast/version.hpp>

#include <thread>

#ifdef SYSTEM_TRACE_HTTP_REQUESTS
#include <sstream>
#endif // SYSTEM_TRACE_HTTP_REQUESTS

namespace initp {
namespace http {
namespace ranged {

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
            sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::ssl_connection::initialize_ssl", "Failed to initialize SSL context. %s (%d)", ec.message().c_str(), ec.value());
            return ec;
        }
    } else {
        ssl.set_verify_mode(boost::asio::ssl::verify_none, ec);
        if (ec) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::ssl_connection::initialize_ssl", "Failed to set no verify mode, %s (%d)", ec.message().c_str(), ec.value());
            return ec;
        }
    }
    return system::err::success;
}

    // Construction

ssl_connection::ssl_connection(
    boost::asio::io_context& ioc,
    boost::asio::ssl::context& ssl,
    size_t max_range_size
):
    connection(),
    file_writer(),
    context_(ioc),
    ssl_(ssl),
    strand_(ioc),
    resolver_(ioc),
    stream_(boost::make_unique<stream_t>(this->context_, this->ssl_)),
    busy_(false),
    connected_(false),
    max_response_size_(0),
    max_range_size_(max_range_size),
    agent_(BOOST_BEAST_VERSION_STRING),
    cache_range_size_(0),
    cache_content_length_(0),
    cache_errors_(0)
{}

ssl_connection::ssl_connection(
    boost::asio::io_context& ioc,
    boost::asio::ssl::context& ssl,
    size_t max_range_size,
    const std::string& user_agent
):
    connection(),
    file_writer(),
    context_(ioc),
    ssl_(ssl),
    strand_(ioc),
    resolver_(ioc),
    stream_(boost::make_unique<stream_t>(this->context_, this->ssl_)),
    busy_(false),
    connected_(false),
    max_response_size_(0),
    max_range_size_(max_range_size),
    agent_(user_agent),
    cache_range_size_(0),
    cache_content_length_(0),
    cache_errors_(0)
{}

    // Public methods

system::error_code ssl_connection::connect(const std::string& host, const std::string& port, const on_result_f& on_result) {

    if (this->connected_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::ranged::ssl_connection::connect", "Already connected");
        return system::err::invalid_state;
    }

    if (this->busy_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::ranged::ssl_connection::connect", "Service is busy");
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
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::ssl_connection::connect", "%s (%d)", ec.message().c_str(), ec.value());
        return system::err::internal_error;
    }

    this->busy_ = true;
    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::ranged::ssl_connection::connect", "Trying to async resolve");

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

system::error_code ssl_connection::disconnect(const on_result_f& on_result) {

    if (!this->connected_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::ranged::ssl_connection::disconnect", "Not connected");
        return system::err::invalid_state;
    }

    if (this->busy_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::ranged::ssl_connection::disconnect", "Service is busy");
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

void ssl_connection::on_resolve(boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type results, const on_result_f& on_result) {

    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::ssl_connection::on_resolve", "%s (%d)", ec.message().c_str(), ec.value());
        this->busy_ = false;
        if (on_result) on_result(ec);
        return;
    }

    // Set a timeout on the operation
    // Bug: https://github.com/boostorg/beast/issues/1695
    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::ranged::ssl_connection::on_resolve", "Trying to set connection timer");
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

void ssl_connection::on_connect(boost::system::error_code ec, const on_result_f& on_result) {

    if (ec) {
        this->close_stream();
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::ssl_connection::on_connect", "%s (%d)", ec.message().c_str(), ec.value());
        this->busy_ = false;
        if (on_result) on_result(ec);
        return;
    }

    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::ranged::ssl_connection::on_connect", "Connected, call handshake");

    // Perform the SSL handshake
    this->stream_->async_handshake(
        boost::asio::ssl::stream_base::client,
        boost::asio::bind_executor(
            this->strand_,
            BIND(on_handshake, _1, on_result)
        )
    );
}

void ssl_connection::on_handshake(boost::system::error_code ec, const on_result_f& on_result) {

    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::ssl_connection::on_handshake", "%s (%d)", ec.message().c_str(), ec.value());
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

void ssl_connection::on_write_head(boost::system::error_code ec, size_t bytes_transferred, const on_progress_f& on_progress, const on_response_f& on_response) {

    boost::ignore_unused(bytes_transferred);

    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::ssl_connection::on_write_head", "%s (%d)", ec.message().c_str(), ec.value());
        this->busy_ = false;
        if (on_response) {
            header_list_t null;
            on_response(ec, boost::beast::http::status::ok, null, 0);
        }
        return;
    }

    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::ranged::ssl_connection::on_write_head", "Data written, trying to set read timer");
    boost::beast::get_lowest_layer(this->stream()).expires_after(std::chrono::seconds(5));

    // Receive the HTTP response
    this->head_.emplace();
    this->head_->skip(true);
    boost::beast::http::async_read_header(
        this->stream(),
        this->buffer_,
        this->head_.get(),
        boost::asio::bind_executor(
            this->strand_,
            BIND(on_read_head, _1, _2, on_progress, on_response)
        )
    );
}

void ssl_connection::on_read_head(boost::system::error_code ec, size_t bytes_transferred, const on_progress_f& on_progress, const on_response_f& on_response) {

    boost::ignore_unused(bytes_transferred);

    // Check error
    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::ssl_connection::on_read_head", "%s (%d)", ec.message().c_str(), ec.value());
        this->busy_ = false;
        if (on_response) {
            header_list_t null;
            on_response(ec, boost::beast::http::status::ok, null, 0);
        }
        return;
    }

    // Build response from parser
    boost::beast::http::response<boost::beast::http::empty_body> response = this->head_->get();
    boost::beast::http::status status = response.result();

    #ifdef SYSTEM_TRACE_HTTP_REQUESTS
    {
        std::stringstream stream;
        stream << "Response: " << std::endl;
        stream << response.result_int() << " " << status << std::endl;
        for (auto const& it : response)
            stream << it.name() << ": " << it.value()
                #if BOOST_VERSION <= 107700
                    .to_string()
                #endif // BOOST_VERSION
            << std::endl;
        stream << "Total data size: " << bytes_transferred << std::endl;
        sys_debug_print(SYSTEM_LEVEL_TRACE, "http::ranged::ssl_connection::on_read_head", "%s", stream.str().c_str());
    }
    #endif // SYSTEM_TRACE_HTTP_REQUESTS

    // Check status
    if (status != boost::beast::http::status::ok) {
        this->busy_ = false;
        if (on_response) {
            header_list_t headers;
            for (auto const& it : response)
                headers[it.name()] = it.value()
                    #if BOOST_VERSION <= 107700
                        .to_string()
                    #endif // BOOST_VERSION
                ;
            on_response(ec, status, headers, 0);
        }
        return;
    }

    // Check content length
    if (response.has_content_length()) {
        size_t begin, end;
        size_t length = tools::string::parse<size_t>(response[boost::beast::http::field::content_length]
            #if BOOST_VERSION <= 107700
                .to_string()
            #endif // BOOST_VERSION
        );
        this->cache_content_length_ = length;
        sys_debug_print(SYSTEM_LEVEL_INFO, "http::ranged::ssl_connection::on_read_head", "Content length: %d; Range size: %d", length, this->cache_range_size_);

        // Open file
        ec = this->open_file(this->cache_filename_);
        if (ec) {
            this->busy_ = false;
            if (on_response) {
                header_list_t null;
                on_response(ec, status, null, 0);
            }
            return;
        }

        // Get range
        std::string range = get_next_range(length, this->cache_range_size_, std::string(), begin, end);
        if (!range.empty()) {
            ec = this->request_range(range, on_progress, on_response);
            if (ec) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::ssl_connection::on_read_head", "Failed to get next range");
                this->busy_ = false;
                if (on_response) {
                    header_list_t null;
                    on_response(ec, boost::beast::http::status::ok, null, 0);
                }
            }
        } else {
            this->close_file();
            this->busy_ = false;
            if (on_response) {
                header_list_t headers;
                for (auto const& it : response)
                    headers[it.name()] = it.value()
                        #if BOOST_VERSION <= 107700
                            .to_string()
                        #endif // BOOST_VERSION
                    ;
                on_response(ec, status, headers, 0);
            }
            return;
        }
    } else {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::ssl_connection::on_read_head", "No content length found");
        this->busy_ = false;
        if (on_response) {
            header_list_t null;
            on_response(ec, boost::beast::http::status::ok, null, 0);
        }
    }
}

void ssl_connection::on_write(boost::system::error_code ec, size_t bytes_transferred, const on_progress_f& on_progress, const on_response_f& on_response) {

    boost::ignore_unused(bytes_transferred);

    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::ssl_connection::on_write", "%s (%d)", ec.message().c_str(), ec.value());
        this->busy_ = false;
        if (on_response) {
            header_list_t null;
            on_response(ec, boost::beast::http::status::ok, null, 0);
        }
        return;
    }

    //sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::ranged::ssl_connection::on_write", "Data written, trying to set read timer");
    boost::beast::get_lowest_layer(this->stream()).expires_after(std::chrono::seconds(5));

    // Receive the HTTP response
    boost::beast::http::async_read(
        this->stream(),
        this->buffer_,
        this->response_,
        boost::asio::bind_executor(
            this->strand_,
            BIND(on_read, _1, _2, on_progress, on_response)
        )
    );
}

void ssl_connection::on_read(boost::system::error_code ec, size_t bytes_transferred, const on_progress_f& on_progress, const on_response_f& on_response) {

    boost::ignore_unused(bytes_transferred);

    // Check error
    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::ssl_connection::on_read", "%s (%d)", ec.message().c_str(), ec.value());
        if (++this->cache_errors_ < 3) {
            this->close();
            // Post reconnect
            boost::asio::post(
                this->strand_,
                [this, on_progress, on_response] (void) -> void {
                    system::error_code ec = this->connect(this->host_, this->port_, [this, on_progress, on_response] (system::error_code ec) -> void {
                        if (ec) {
                            sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::ssl_connection::on_read::on_reconnect", "%s (%d)", ec.message().c_str(), ec.value());
                            this->busy_ = false;
                            if (on_response) {
                                header_list_t null;
                                on_response(ec, boost::beast::http::status::ok, null, 0);
                            }
                        } else {
                            ec = this->request_range(this->cache_range_, on_progress, on_response);
                            if (ec) {
                                this->busy_ = false;
                                if (on_response) {
                                    header_list_t null;
                                    on_response(ec, boost::beast::http::status::ok, null, 0);
                                }
                            }
                        }
                    });
                    if (ec) {
                        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::ssl_connection::on_read::reconnect", "%s (%d)", ec.message().c_str(), ec.value());
                        this->busy_ = false;
                        if (on_response) {
                            header_list_t null;
                            on_response(ec, boost::beast::http::status::ok, null, 0);
                        }
                    }
                }
            );
            return;
        } else {
            this->busy_ = false;
            if (on_response) {
                header_list_t null;
                on_response(ec, boost::beast::http::status::ok, null, 0);
            }
        }
        return;
    } else {
        this->cache_errors_ = 0;
    }

    // Check status
    boost::beast::http::status status = this->response_.result();
    if (status != boost::beast::http::status::partial_content) {
        this->busy_ = false;
        if (on_response) {
            header_list_t headers;
            for (auto const& it : this->response_)
                headers[it.name()] = it.value()
                    #if BOOST_VERSION <= 107700
                        .to_string()
                    #endif // BOOST_VERSION
                ;
            on_response(ec, status, headers, 0);
        }
        return;
    }

    // Check content range
    auto it = this->response_.find(boost::beast::http::field::content_range);
    if (it != this->response_.end()) {

        // Check body size
        if (this->response_.body().size > this->max_response_size_) {
            sys_debug_print(SYSTEM_LEVEL_FATAL, "http::ranged::ssl_connection::on_read", "Segmentation fault");
            this->busy_ = false;
            if (on_response) {
                header_list_t null;
                on_response(system::err::internal_error, status, null, 0);
            }
            return;
        }
        size_t size = this->max_response_size_ - this->response_.body().size;

        // Buffer to file
        ec = this->write_file(&this->response_data_[0], size);
        if (ec) {
            this->close_file();
            this->busy_ = false;
            if (on_response) {
                header_list_t null;
                on_response(ec, boost::beast::http::status::ok, null, 0);
            }
            return;
        }

        // Next range
        size_t begin = 0, end = 0;
        std::string range = it->value()
            #if BOOST_VERSION <= 107700
                .to_string()
            #endif // BOOST_VERSION
        ;
        range = get_next_range(this->cache_content_length_, this->cache_range_size_, range, begin, end);
        #ifdef SYSTEM_TRACE_HTTP_REQUESTS
        sys_debug_print(SYSTEM_LEVEL_INFO, "http::ranged::ssl_connection::on_read", "Read complete. Begin: %d; End: %d", begin, end);
        #endif // SYSTEM_TRACE_HTTP_REQUESTS
        if (on_progress) on_progress(end + 1, this->cache_content_length_);
        if (range.empty()) {
            this->close_file();
            this->busy_ = false;
            sys_debug_print(SYSTEM_LEVEL_INFO, "http::ranged::ssl_connection::on_read", "Download complete");
            if (on_response) {
                header_list_t headers;
                for (auto const& it : this->response_)
                    headers[it.name()] = it.value()
                        #if BOOST_VERSION <= 107700
                            .to_string()
                        #endif // BOOST_VERSION
                    ;
                on_response(ec, boost::beast::http::status::ok, headers, this->cache_content_length_);
            }
        } else {
            ec = this->request_range(range, on_progress, on_response);
            if (ec) {
                sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::ssl_connection::on_read", "Failed to get next range");
                this->busy_ = false;
                if (on_response) {
                    header_list_t null;
                    on_response(ec, boost::beast::http::status::ok, null, 0);
                }
            }
        }
    } else {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::ssl_connection::on_read", "No content range found");
        this->busy_ = false;
        if (on_response) {
            header_list_t null;
            on_response(system::err::bad_data, boost::beast::http::status::ok, null, 0);
        }
    }
}

void ssl_connection::on_shutdown(boost::system::error_code ec, const on_result_f& on_result) {
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
