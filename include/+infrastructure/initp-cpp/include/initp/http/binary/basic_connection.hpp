#ifndef INITPLUS_HTTP_BINARY_BASIC_CONNECTION_HPP_INCLUDED
#define INITPLUS_HTTP_BINARY_BASIC_CONNECTION_HPP_INCLUDED

#include <initp/http/binary/connection.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace initp {
namespace http {
namespace binary {

class basic_connection:
    public std::enable_shared_from_this<basic_connection>,
    public http::binary::connection {

private: // Private types
    typedef basic_connection self_type;

public: // Construction
    basic_connection(boost::asio::io_context&);
    basic_connection(boost::asio::io_context&, const std::string&);
    basic_connection(const self_type&) = delete;
    virtual ~basic_connection(void) = default;

public: // Public methods
    virtual system::error_code connect(const std::string&, const std::string&, const on_result_f&);
    virtual system::error_code get(const std::string&, const header_list_t&, const on_response_f&);
    virtual system::error_code post(const std::string&, const header_list_t&, const std::vector<uint8_t>&, const on_response_f&);
    virtual system::error_code post(const std::string&, const header_list_t&, const uint8_t*, size_t, const on_response_f&);
    virtual void close(void);

protected: // Private methods
    void on_resolve(boost::system::error_code, boost::asio::ip::tcp::resolver::results_type, const on_result_f&);
    void on_connect(boost::system::error_code, const on_result_f&);
    void on_write(boost::system::error_code, std::size_t bytes_transferred, const on_response_f&);
    void on_read(boost::system::error_code, std::size_t bytes_transferred, const on_response_f&);
    void close_stream(void);

public: // Public properties
    virtual bool connected(void) const;
    virtual bool busy(void) const;

private: // Private properties
    boost::beast::tcp_stream& stream(void);

protected: // Private fields
    boost::asio::io_context& context_;
    boost::asio::io_context::strand strand_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::beast::flat_buffer buffer_;
    std::unique_ptr<boost::beast::tcp_stream> stream_;
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

#endif // INITPLUS_HTTP_BINARY_BASIC_CONNECTION_HPP_INCLUDED
