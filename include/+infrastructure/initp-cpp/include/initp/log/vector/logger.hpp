#ifndef INITPLUS_LOG_VECTOR_LOGGER_HPP_INCLUDED
#define INITPLUS_LOG_VECTOR_LOGGER_HPP_INCLUDED

#include <initp/socket/raw/connection.hpp>
#include <initp/log/vector/event.pb.h>

#include <initp/system/debug.hpp>
#include <initp/utils/macro.hpp>
#include <initp/system/time.hpp>

#include <boost/asio/post.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/ssl/context.hpp>
#include <functional>
#include <memory>
#include <atomic>
#include <thread>
#include <queue>
#include <tuple>
#include <cstdint>

namespace initp {
namespace log {
namespace vector {

class logger:
    public std::enable_shared_from_this<logger> {

private: // Private types
    typedef logger self_type;

protected: // Protected types
    typedef google::protobuf::Map<std::string, event::Value> message_t;

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
        const std::string& device_name,
        const std::string& device_id,
        system::time_t delay = 3000,
        size_t max_queue_size = 1000,
        size_t max_batch_size = 20
    );
    void reinitialize(
        const std::string& device_name,
        const std::string& device_id
    );
    void start(bool = true);
    void stop(void);
    void flush(void);
    void enable(void);
    void disable(void);
    void queue_message(message_t&);

public: // Log templates

    template<typename E, typename T, typename... Args>
    void log_message(uint8_t level, const E& e, const T& what, const Args&... args) {
        if (!this->started_ || !this->enabled_) return;
        message_t message;
        {
            event::Value value;
            system::time_t now = system::time::milliseconds();
            value.mutable_timestamp()->set_seconds(now / 1000);
            value.mutable_timestamp()->set_nanos((now % 1000) * 1000000);
            //*value.mutable_timestamp() = google::protobuf::util::TimeUtil::GetCurrentTime();
            message["timestamp"] = value;
        }
        {
            event::Value value;
            value.set_raw_bytes("logs");
            message["type"] = value;
        }
        {
            event::Value value;
            value.set_raw_bytes(this->device_name_);
            message["device"] = value;
        }
        {
            event::Value value;
            value.set_raw_bytes(this->device_id_);
            message["device_id"] = value;
        }
        {
            event::Value value;
            value.set_integer(level);
            message["level"] = value;
        }
        {
            std::stringstream stream;
            stream << e;
            std::string str = stream.str();
            if (!str.empty()) {
                event::Value value;
                value.set_raw_bytes(str);
                message["error"] = value;
            }
        }
        {
            event::Value value;
            value.set_raw_bytes(what);
            message["what"] = value;
        }
        {
            event::Value value;
            std::stringstream stream;
            (int[]) { (stream << args, 0)... };
            value.set_raw_bytes(stream.str());
            message["message"] = value;
        }
        boost::asio::post(
            boost::asio::bind_executor(
                this->strand_,
                RBIND(push_message, message)
            )
        );
    }

    template<typename E, typename T, typename... Args>
    void fatal(const E& e, const T& what, const Args&... args) {
        return this->log_message(SYSTEM_LEVEL_FATAL, e, what, args...);
    }

    template<typename E, typename T, typename... Args>
    void error(const E& e, const T& what, const Args&... args) {
        return this->log_message(SYSTEM_LEVEL_ERROR, e, what, args...);
    }

    template<typename E, typename T, typename... Args>
    void warning(const E& e, const T& what, const Args&... args) {
        return this->log_message(SYSTEM_LEVEL_WARNING, e, what, args...);
    }

    template<typename T, typename... Args>
    void info(const T& what, const Args&... args) {
        return this->log_message(SYSTEM_LEVEL_INFO, EMPTY, what, args...);
    }

    template<typename T, typename... Args>
    void debug(const T& what, const Args&... args) {
        return this->log_message(SYSTEM_LEVEL_DEBUG, EMPTY, what, args...);
    }

    template<typename T, typename... Args>
    void trace(const T& what, const Args&... args) {
        return this->log_message(SYSTEM_LEVEL_TRACE, EMPTY, what, args...);
    }

private: // Private methods
    void push_message(message_t&);
    void batch_messages(void);
    void send_messages(void);
    void do_flush(void);
    void set_enabled(bool);

private: // Callbacks
    void on_connect(boost::system::error_code ec);
    void on_write(boost::system::error_code ec);
    void on_timer(boost::system::error_code ec);

public: // Public properties
    bool started(void) const;
    bool enabled(void) const;
    const std::string& device_name(void) const;
    const std::string& device_id(void) const;

private: // Private fields
    const std::string EMPTY;
    std::string device_name_;
    std::string device_id_;
    std::atomic<bool> started_;
    std::atomic<bool> enabled_;
    std::atomic<bool> flush_;
    std::unique_ptr<std::thread> thread_;
    size_t max_queue_size_;
    size_t max_batch_size_;
    std::string remote_host_;
    std::string remote_port_;
    boost::asio::io_context context_;
    boost::asio::io_context::strand strand_;
    boost::asio::ssl::context ssl_context_;
    boost::asio::steady_timer timer_;
    initp::socket::raw::connection::ptr connection_;
    std::vector<uint8_t> data_to_send_;
    std::queue<message_t> queue_;
    bool ssl_context_used_;
    system::time_t delay_;
    int32_t sequence_id_;
};

}}}

#endif // INITPLUS_LOG_VECTOR_LOGGER_HPP_INCLUDED
