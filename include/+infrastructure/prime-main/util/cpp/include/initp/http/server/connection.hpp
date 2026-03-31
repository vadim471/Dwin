#ifndef INITPLUS_HTTP_SERVER_CONNECTION_HPP_INCLUDED
#define INITPLUS_HTTP_SERVER_CONNECTION_HPP_INCLUDED

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/steady_timer.hpp>

#include <memory>
#include <vector>

namespace initp {
namespace http {

class session_pool;
class server_config;
class http_queue;

/**
 * \brief Handles an HTTP server connection.
 */
class http_connection:
    public std::enable_shared_from_this<http_connection> {
    friend class http_queue;
    using tcp = boost::asio::ip::tcp;

private: // Private types
    typedef http_connection self_type;
    typedef boost::asio::ssl::stream<tcp::socket&> ssl_stream_t;

public: // Public types
    typedef std::shared_ptr<self_type> ptr;
    typedef std::weak_ptr<self_type> wptr;

public: // Construction
    /**
     * \brief Take ownership of the socket.
     */
    explicit http_connection(
        boost::asio::io_context&,
        const server_config&,
        session_pool&,
        tcp::socket&&
    );
    explicit http_connection(
        boost::asio::io_context&,
        boost::asio::ssl::context&,
        const server_config&,
        session_pool&,
        tcp::socket&&
    );
    ~http_connection(void);

public: // Public methods
    /**
     * \brief Start the asynchronous operation.
     */
    void run(void);
    void do_read(void);
    /**
     * \brief Called when the timer expires.
     */
    void on_timer(boost::system::error_code err);
    void on_read(boost::system::error_code err);
    void on_write(boost::system::error_code err, bool close);
    void do_close(void);

private: // Private methods
    void on_handshake(boost::system::error_code err);
    void on_shutdown(boost::system::error_code err);

public: // Public properties
    const server_config& config(void) const;
    session_pool& pool(void);
    boost::asio::io_context::strand& strand(void);

private: // Private fields
    const server_config& config_;
    session_pool& pool_;
    tcp::socket socket_;
    std::unique_ptr<ssl_stream_t> ssl_;
    boost::asio::io_context::strand strand_;
    boost::asio::steady_timer timer_;
    boost::beast::flat_buffer buffer_;
    boost::beast::http::request<boost::beast::http::string_body> request_;
    std::unique_ptr<http_queue> queue_;
};

}}

#endif // INITPLUS_HTTP_SERVER_CONNECTION_HPP_INCLUDED
