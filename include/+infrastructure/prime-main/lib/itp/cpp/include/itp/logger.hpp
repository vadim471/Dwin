#ifndef ITP_LOGGER_HPP_INCLUDED
#define ITP_LOGGER_HPP_INCLUDED

#include <initp/system/macro.hpp>

#include <itp/itp.hpp>

#include <cstring>
#include <algorithm>
#include <functional>
#include <sstream>
#include <memory>
#include <mutex>
#include <tuple>

namespace itp {

class logger:
    public std::enable_shared_from_this<logger> {

private: // Private types
    typedef logger self_type;

public: // Public types
    typedef std::shared_ptr<self_type> ptr;

public: // Construction
    logger(void):
        root_(nullptr),
        enabled_(false),
        busy_(false)
    {}
    logger(const self_type&) = delete;
    virtual ~logger(void) = default;

public: // Parse assistants
    static inline void ltrim(std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [] (int ch) {
            return !std::isspace(ch);
        }));
    }
    static inline void rtrim(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [] (int ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }
    static std::tuple<std::string, std::string> parse_trace(const char* message) {
        if (!message) return std::make_tuple(std::string(), std::string());
        size_t length = strlen(message);
        std::string body;
        std::string what;
        if (length > 0) {
            for (size_t i = 0; i < length; ++i) {
                if (message[i] == '>') {
                    if (i > 0) {
                        what.assign(message, i);
                    }
                    if (i + 1 < length) {
                        body.assign(message + i + 1);
                        ltrim(body);
                        rtrim(body);
                    }
                    break;
                }
            }
        }
        if (what.empty()) what = "unknown source";
        if (body.empty()) body = std::string(message);
        return std::make_tuple(what, body);
    }
    static std::tuple<std::string, std::string, std::string> parse_error(int error, const char* category, const char* message) {
        if (!message) return std::make_tuple(std::string(), std::string(), std::string());
        size_t length = strlen(message);
        std::string body;
        std::string what;
        if (length > 0) {
            for (size_t i = 0; i < length; ++i) {
                if (message[i] == '>') {
                    if (i > 0) {
                        what.assign(message, i);
                    }
                    if (i + 1 < length) {
                        body.assign(message + i + 1);
                        ltrim(body);
                        rtrim(body);
                    }
                    break;
                }
            }
        }
        if (what.empty()) what = "unknown source";
        if (body.empty()) body = std::string(message);
        std::stringstream stream;
        stream << category << ":" << error;
        return std::make_tuple(stream.str(), what, body);
    }

public: // Log templates

    template<typename T, typename... Args>
    void trace(uint8_t level, const T& what, const Args&... args) {
        if (!this->root_ || !this->enabled_) return;
        {
            LOCK_RMUTEX(this->mutex_);
            if (this->busy_) return;
            this->busy_ = true;
        }
        {
            std::stringstream stream;
            stream << what;
            std::string target = stream.str();
            stream.str("");
            (int[]) { (stream << args, 0)... };
            std::string message = stream.str();
            itp::frame::uptr event(new (std::nothrow) itp::frame(ITP_CMD_DEBUG_MESSAGE));
            if (!event) goto EXIT_TRACE;
            itp_error_code_t errc = event->write_value(level);
            if (errc) goto EXIT_TRACE;
            errc = event->write_string(target);
            if (errc) goto EXIT_TRACE;
            errc = event->write_wide_string(message);
            if (errc) goto EXIT_TRACE;
            if (this->root_->mutex().try_lock()) {
                this->root_->push_event(std::move(event));
                this->root_->mutex().unlock();
            }
        }
    EXIT_TRACE:
        this->busy_ = false;
    }

    template<typename E, typename T, typename... Args>
    void error(uint8_t level, const E& e, const T& what, const Args&... args) {
        if (!this->root_ || !this->enabled_) return;
        {
            LOCK_RMUTEX(this->mutex_);
            if (this->busy_) return;
            this->busy_ = true;
        }
        {
            std::stringstream stream;
            stream << what;
            std::string target = stream.str();
            stream.str("");
            stream << "<" << e << "> ";
            (int[]) { (stream << args, 0)... };
            std::string message = stream.str();
            itp::frame::uptr event(new (std::nothrow) itp::frame(ITP_CMD_DEBUG_MESSAGE));
            if (!event) goto EXIT_ERROR;
            itp_error_code_t errc = event->write_value(level);
            if (errc) goto EXIT_ERROR;
            errc = event->write_string(target);
            if (errc) goto EXIT_ERROR;
            errc = event->write_wide_string(message);
            if (errc) goto EXIT_ERROR;
            if (this->root_->mutex().try_lock()) {
                this->root_->push_event(std::move(event));
                this->root_->mutex().unlock();
            }
        }
    EXIT_ERROR:
        this->busy_ = false;
    }

public: // Public methods
    void start(itp::root* node, bool enable = true) {
        this->root_ = node;
        if (enable) this->enable();
    }
    void stop(void) {
        this->root_ = nullptr;
    }
    void enable(void) {
        this->enabled_ = true;
    }
    void disable(void) {
        this->enabled_ = false;
    }

private: // Private fields
    itp::root* root_;
    std::recursive_mutex mutex_;
    bool enabled_;
    bool busy_;
};

}

#endif // ITP_LOGGER_HPP_INCLUDED
