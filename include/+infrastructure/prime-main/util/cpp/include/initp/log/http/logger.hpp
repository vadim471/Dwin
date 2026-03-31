#ifndef INITPLUS_LOG_HTTP_LOGGER_HPP_INCLUDED
#define INITPLUS_LOG_HTTP_LOGGER_HPP_INCLUDED

#include <initp/system/debug.hpp>
#include <initp/system/time.hpp>
#include <initp/system/macro.hpp>

#include <boost/asio/post.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/ssl/context.hpp>

#include <boost/beast/http/status.hpp>

#include <functional>
#include <memory>
#include <atomic>
#include <thread>
#include <queue>
#include <tuple>
#include <cstdint>

namespace initp {

namespace http {
namespace binary {
class connection;
}}

namespace log {
namespace http {

class logger:
    public std::enable_shared_from_this<logger> {

private: // Private types
    typedef logger self_type;
    typedef std::tuple<uint8_t, std::string, std::string, system::time_t> log_message_t;

public: // Public types
    typedef std::shared_ptr<self_type> ptr;
    typedef std::unique_ptr<self_type> uptr;

public: // Construction
    logger(void);
    logger(const self_type&) = delete;
    virtual ~logger(void) = default;

public: // Public methods
    void set_certificate(
        const std::string& ca,
        const std::string& cl,
        const std::string& key
    );
    void initialize(
        const std::string& host,
        const std::string& port,
        const std::string& target,
        const std::string& local_name,
        const std::string& application,
        system::time_t = 3000,
        size_t max_queue_size = 1000,
        size_t max_batch_size = 20
    );
    void reinitialize(system::time_t, const std::string&, const std::string&);
    void start(bool = true);
    void stop(void);
    void flush(void);
    void enable(void);
    void disable(void);
    void queue_message(
        uint8_t level,
        const std::string& what,
        const std::string& message,
        system::time_t time
    );

public: // Log templates

    template<typename T, typename... Args>
    void trace(uint8_t level, const T& what, const Args&... args) {
        if (!this->started_) return;
        std::stringstream stream;
        stream << what;
        std::string target = stream.str();
        stream.str("");
        (int[]) { (stream << args, 0)... };
        std::string message = stream.str();
        boost::asio::post(
            boost::asio::bind_executor(
                this->strand_,
                RBIND(push_message, level, target, message, system::time::now())
            )
        );
    }

    template<typename E, typename T, typename... Args>
    void error(uint8_t level, const E& e, const T& what, const Args&... args) {
        if (!this->started_) return;
        std::stringstream stream;
        stream << what;
        std::string target = stream.str();
        stream.str("");
        stream << "<" << e << "> ";
        (int[]) { (stream << args, 0)... };
        std::string message = stream.str();
        boost::asio::post(
            boost::asio::bind_executor(
                this->strand_,
                RBIND(push_message, level, target, message, system::time::now())
            )
        );
    }

private: // Private methods
    void push_message(
        uint8_t level,
        const std::string& what,
        const std::string& message,
        system::time_t time
    );
    void batch_messages(void);
    void send_messages(void);
    void do_flush(void);
    void set_enabled(bool);

private: // Callbacks
    void on_connect(boost::system::error_code ec);
    void on_response(boost::system::error_code ec, boost::beast::http::status status, const uint8_t* body, size_t size);
    void on_timer(boost::system::error_code ec);

private: // Time functions
    int64_t to_server_time_format(system::time_t time);
    int64_t to_local_time(system::time_t time);

public: // Public properties
    bool started(void) const;
    bool enabled(void) const;
    system::time_t start_time(void) const;
    const std::string& local_host(void) const;
    const std::string& application_name(void) const;

private: // Private fields
    system::time_t start_time_;
    std::unique_ptr<std::thread> thread_;
    size_t max_queue_size_;
    size_t max_batch_size_;
    std::string remote_host_;
    std::string remote_port_;
    std::string remote_target_;
    std::string local_host_;
    std::string application_name_;
    boost::asio::io_context context_;
    boost::asio::io_context::strand strand_;
    boost::asio::ssl::context ssl_context_;
    boost::asio::steady_timer timer_;
    std::shared_ptr<::initp::http::binary::connection> connection_;
    std::vector<uint8_t> data_to_send_;
    std::queue<log_message_t> queue_;
    std::atomic<bool> started_;
    std::atomic<bool> enabled_;
    std::atomic<bool> flush_;
    system::time_t delay_;
    int32_t sequence_id_;
};

}}}

#endif // INITPLUS_LOG_HTTP_LOGGER_HPP_INCLUDED
