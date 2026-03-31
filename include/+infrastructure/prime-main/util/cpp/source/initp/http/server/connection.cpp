#include <initp/http/server/session.hpp>
#include <initp/http/server/handler.hpp>
#include <initp/http/server/queue.hpp>
#include <initp/system/macro.hpp>
#include <initp/system/cdebug>

namespace initp {
namespace http {

using namespace std::placeholders;

    // Handler call

void http_queue::operator()(boost::beast::http::request<boost::beast::http::string_body>&& request) {

    // Allocate and store the work
    this->items_.push_back(boost::make_unique<request_work>(this->parent_));

    (*std::make_shared<http_handler<http_queue>>(
        this->parent_.shared_from_this(),
        std::move(request),
        *this,
        this->items_.back().get()
    ))();
}

    // Class http_connection

http_connection::http_connection(
    boost::asio::io_context& context,
    const server_config& config,
    session_pool& pool,
    tcp::socket&& socket
)
    : config_(config)
    , pool_(pool)
    , socket_(std::move(socket))
    , strand_(context)
    , timer_(context, (std::chrono::steady_clock::time_point::max)())
    , queue_(new http_queue(*this))
{
    sys_debug_print(SYSTEM_LEVEL_TRACE, "http::http_connection::http_connection", "Constructed");
}

http_connection::http_connection(
    boost::asio::io_context& context,
    boost::asio::ssl::context& ssl_context,
    const server_config& config,
    session_pool& pool,
    tcp::socket&& socket
)
    : config_(config)
    , pool_(pool)
    , socket_(std::move(socket))
    , strand_(context)
    , timer_(context, (std::chrono::steady_clock::time_point::max)())
    , queue_(new http_queue(*this))
{
    this->ssl_.reset(new ssl_stream_t(socket_, ssl_context));
    sys_debug_print(SYSTEM_LEVEL_TRACE, "http::http_connection::http_connection", "Constructed with SSL");
}

http_connection::~http_connection(void) {
    sys_debug_print(SYSTEM_LEVEL_TRACE, "http::http_connection::~http_connection", "Destructed");
}

void http_connection::run(void) {

    // Perform the SSL handshake
    if (this->ssl_) {
        this->ssl_->async_handshake(
            boost::asio::ssl::stream_base::server,
            boost::asio::bind_executor(
                this->strand_,
                BIND(on_handshake, _1)
            )
        );
        return;
    }

    // Run the timer. The timer is operated
    // continuously, this simplifies the code.
    this->on_timer({});
    this->do_read();
}

void http_connection::on_handshake(boost::system::error_code ec) {
    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::http_connection::on_handshake", "Handshake failed. %s", ec.message().c_str());
        return;
    }
    this->do_read();
}

void http_connection::do_read(void) {

    // Set the timer
    this->timer_.expires_after(std::chrono::seconds(15));

    // Make the request empty before reading,
    // otherwise the operation behavior is undefined.
    this->request_ = {};

    // Read a request
    if (this->ssl_) {
        boost::beast::http::async_read(
            *this->ssl_,
            this->buffer_,
            this->request_,
            boost::asio::bind_executor(
                this->strand_,
                BIND(on_read, _1)
            )
        );
    } else {
        boost::beast::http::async_read(
            this->socket_,
            this->buffer_,
            this->request_,
            boost::asio::bind_executor(
                this->strand_,
                BIND(on_read, _1)
            )
        );
    }
}

void http_connection::on_timer(boost::system::error_code ec) {

    if (ec && ec != boost::asio::error::operation_aborted) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::http_connection::on_timer", "Operation failed. %s", ec.message().c_str());
        return;
    }

    // Verify that the timer really expired since the deadline may have moved.
    if (this->timer_.expiry() <= std::chrono::steady_clock::now()) {
        // Closing the socket cancels all outstanding operations. They
        // will complete with boost::asio::error::operation_aborted
        this->socket_.shutdown(tcp::socket::shutdown_both, ec);
        this->socket_.close(ec);
        return;
    }

    // Wait on the timer
    this->timer_.async_wait(
        boost::asio::bind_executor(
            this->strand_,
            BIND(on_timer, _1)
        )
    );
}

void http_connection::on_read(boost::system::error_code ec) {

    // Happens when the timer closes the socket or on "ssl::error::stream_truncated" error, also known as an SSL "short read",
    // indicates the peer closed the connection without performing the required closing handshake
    if (ec == boost::asio::error::operation_aborted || ec == boost::asio::ssl::error::stream_truncated)
        return;

    // This means they closed the connection
    if (ec == boost::beast::http::error::end_of_stream)
        return this->do_close();

    // Any other error
    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::http_connection::on_read", "Operation failed. %s", ec.message().c_str());
        return;
    }

    // Send the response
    (*this->queue_)(std::move(this->request_));

    // If we aren't at the queue limit, try to pipeline another request
    if (!this->queue_->is_full()) this->do_read();
}

void http_connection::on_write(boost::system::error_code ec, bool close) {

    // Happens when the timer closes the socket or on "ssl::error::stream_truncated" error, also known as an SSL "short read",
    // indicates the peer closed the connection without performing the required closing handshake
    if (ec == boost::asio::error::operation_aborted || ec == boost::asio::ssl::error::stream_truncated)
        return;

    // Any other error
    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::http_connection::on_write", "Operation failed. %s", ec.message().c_str());
        return;
    }

    if (close) {
        // This means we should close the connection, usually because
        // the response indicated the "Connection: close" semantic.
        return this->do_close();
    }

    // Inform the queue that a write completed
    if (this->queue_->on_write()) {
        // Read another request
        return this->do_read();
    }
}

void http_connection::do_close(void) {

    // Perform the SSL shutdown
    if (this->ssl_) {
        this->ssl_->async_shutdown(
            boost::asio::bind_executor(
                this->strand_,
                BIND(on_shutdown, _1)
            )
        );
        return;
    }

    // Send a TCP shutdown
    boost::system::error_code ec;
    this->socket_.shutdown(tcp::socket::shutdown_send, ec);

    // At this point the connection is closed gracefully
}

void http_connection::on_shutdown(boost::system::error_code ec) {
    if (ec) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::http_connection::on_shutdown", "Operation failed. %s", ec.message().c_str());
        return;
    }

    // At this point the connection is closed gracefully
}

    // Public properties

const server_config& http_connection::config(void) const {
    return this->config_;
}

session_pool& http_connection::pool(void) {
    return this->pool_;
}

boost::asio::io_context::strand& http_connection::strand(void) {
    return this->strand_;
}

}}
