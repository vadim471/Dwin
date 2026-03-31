#ifndef INITPLUS_HTTP_BINARY_SSL_CONNECTION_HPP_INCLUDED
#define INITPLUS_HTTP_BINARY_SSL_CONNECTION_HPP_INCLUDED

#include <initp/http/binary/connection.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace initp {
namespace http {
namespace binary {

class ssl_connection:
    public std::enable_shared_from_this<ssl_connection>,
    public http::binary::connection {

private: // Private types
    typedef ssl_connection self_type;
    typedef boost::beast::ssl_stream<boost::beast::tcp_stream> stream_t;

public: // Static methods
    static system::error_code initialize_ssl(boost::asio::ssl::context&, const std::string&, const std::string&, const std::string&);

public: // Construction
    ssl_connection(boost::asio::io_context&, boost::asio::ssl::context&);
    ssl_connection(boost::asio::io_context&, boost::asio::ssl::context&, const std::string&);
    ssl_connection(const self_type&) = delete;
    virtual ~ssl_connection(void) = default;

public: // Public methods
    virtual system::error_code connect(const std::string&, const std::string&, const on_result_f&);
    system::error_code disconnect(const on_result_f&);
    virtual system::error_code get(const std::string&, const header_list_t&, const on_response_f&);
    virtual system::error_code post(const std::string&, const header_list_t&, const std::vector<uint8_t>&, const on_response_f&);
    virtual system::error_code post(const std::string&, const header_list_t&, const uint8_t*, size_t, const on_response_f&);
    virtual void close(void);

protected: // Private methods
    void on_resolve(boost::system::error_code, boost::asio::ip::tcp::resolver::results_type, const on_result_f&);
    void on_connect(boost::system::error_code, const on_result_f&);
    void on_handshake(boost::system::error_code, const on_result_f&);
    void on_write(boost::system::error_code, std::size_t bytes_transferred, const on_response_f&);
    void on_read(boost::system::error_code, std::size_t bytes_transferred, const on_response_f&);
    void on_shutdown(boost::system::error_code, const on_result_f&);
    void close_stream(void);

public: // Public properties
    virtual bool connected(void) const;
    virtual bool busy(void) const;

private: // Private properties
    stream_t& stream(void);

protected: // Private fields
    boost::asio::io_context& context_;
    boost::asio::ssl::context& ssl_;
    boost::asio::io_context::strand strand_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::beast::flat_buffer buffer_;
    std::unique_ptr<stream_t> stream_;
    boost::beast::http::request<boost::beast::http::buffer_body> request_;
    boost::beast::http::request<boost::beast::http::empty_body> empty_request_;
    boost::beast::http::response<boost::beast::http::buffer_body> response_;
    std::vector<uint8_t> request_data_;
    std::vector<uint8_t> response_data_;
    bool busy_;
    bool connected_;
    size_t max_response_size_;
    std::string agent_;
    std::string host_;
    std::string port_;
};

}}}

#endif // INITPLUS_HTTP_BINARY_SSL_CONNECTION_HPP_INCLUDED
