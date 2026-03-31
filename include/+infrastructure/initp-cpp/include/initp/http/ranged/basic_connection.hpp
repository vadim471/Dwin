#ifndef INITPLUS_HTTP_RANGED_BASIC_CONNECTION_HPP_INCLUDED
#define INITPLUS_HTTP_RANGED_BASIC_CONNECTION_HPP_INCLUDED

#include <initp/http/ranged/connection.hpp>
#include <initp/http/ranged/file_writer.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/optional.hpp>

namespace initp {
namespace http {
namespace ranged {

class basic_connection:
    public std::enable_shared_from_this<basic_connection>,
    public http::ranged::connection,
    public http::ranged::file_writer {

private: // Private types
    typedef basic_connection self_type;

public: // Construction
    basic_connection(boost::asio::io_context&, size_t);
    basic_connection(boost::asio::io_context&, size_t, const std::string&);
    basic_connection(const self_type&) = delete;
    virtual ~basic_connection(void) = default;

public: // Public methods
    virtual system::error_code connect(const std::string&, const std::string&, const on_result_f&);
    virtual system::error_code get(const std::string&, const header_list_t&, const std::string&, const on_progress_f&, const on_response_f&);
    virtual void close(void);

protected: // Private methods
    system::error_code request_range(const std::string&, const on_progress_f&, const on_response_f&);
    void on_resolve(boost::system::error_code, boost::asio::ip::tcp::resolver::results_type, const on_result_f&);
    void on_connect(boost::system::error_code, const on_result_f&);
    void on_write_head(boost::system::error_code, size_t bytes_transferred, const on_progress_f&, const on_response_f&);
    void on_read_head(boost::system::error_code, size_t bytes_transferred, const on_progress_f&, const on_response_f&);
    void on_write(boost::system::error_code, size_t bytes_transferred, const on_progress_f&, const on_response_f&);
    void on_read(boost::system::error_code, size_t bytes_transferred, const on_progress_f&, const on_response_f&);
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
    boost::beast::http::request<boost::beast::http::empty_body> request_;
    boost::optional<boost::beast::http::response_parser<boost::beast::http::empty_body>> head_;
    boost::beast::http::response<boost::beast::http::buffer_body> response_;
    std::vector<uint8_t> request_data_;
    std::vector<uint8_t> response_data_;
    bool busy_;
    bool connected_;
    size_t max_response_size_;
    size_t max_range_size_;
    std::string agent_;
    std::string host_;
    std::string port_;
    boost::beast::http::verb cache_method_;
    size_t cache_range_size_;
    size_t cache_content_length_;
    size_t cache_errors_;
    std::string cache_target_;
    std::string cache_filename_;
    std::string cache_range_;
    header_list_t cache_headers_;
};

}}}

#endif // INITPLUS_HTTP_RANGED_BASIC_CONNECTION_HPP_INCLUDED
