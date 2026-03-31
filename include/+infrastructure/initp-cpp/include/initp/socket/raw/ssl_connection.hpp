#ifndef INITPLUS_SOCKET_RAW_SSL_CONNECTION_HPP_INCLUDED
#define INITPLUS_SOCKET_RAW_SSL_CONNECTION_HPP_INCLUDED

#include <initp/socket/raw/connection.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>

namespace initp {
namespace socket {
namespace raw {

class ssl_connection:
    public std::enable_shared_from_this<ssl_connection>,
    public socket::raw::connection {

private: // Private types
    typedef ssl_connection self_type;
    typedef boost::asio::ip::tcp::socket socket_t;
    typedef boost::asio::ssl::stream<socket_t> stream_t;

public: // Static methods
    static system::error_code initialize_ssl(boost::asio::ssl::context&, const std::string&, const std::string&, const std::string&);

public: // Construction
    ssl_connection(boost::asio::io_context&, boost::asio::ssl::context&);
    ssl_connection(const self_type&) = delete;
    virtual ~ssl_connection(void) = default;

public: // Public methods
    virtual system::error_code connect(const std::string&, const std::string&, on_result_f);
    system::error_code disconnect(on_result_f);
    virtual system::error_code write(const std::string&, on_result_f);
    virtual system::error_code write(const std::vector<uint8_t>&, on_result_f);
    virtual system::error_code write(const uint8_t*, size_t, on_result_f);
    virtual void close(void);

protected: // Private methods
    void on_resolve(boost::system::error_code, boost::asio::ip::tcp::resolver::results_type, on_result_f);
    void on_connect(boost::system::error_code, on_result_f);
    void on_handshake(boost::system::error_code, on_result_f);
    void on_write(boost::system::error_code, std::size_t bytes_transferred, on_result_f);
    void on_shutdown(boost::system::error_code, on_result_f);
    void on_deadline(boost::system::error_code);
    void close_stream(void);

public: // Public properties
    virtual bool connected(void) const;
    virtual bool busy(void) const;

private: // Private properties
    socket_t& socket(void);
    stream_t& stream(void);

protected: // Private fields
    boost::asio::io_context& context_;
    boost::asio::ssl::context& ssl_;
    boost::asio::io_context::strand strand_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::asio::steady_timer deadline_;
    std::unique_ptr<stream_t> stream_;
    std::vector<uint8_t> request_;
    bool busy_;
    bool connected_;
    std::string host_;
    std::string port_;
};

}}}

#endif // INITPLUS_SOCKET_RAW_SSL_CONNECTION_HPP_INCLUDED
