#ifndef INITP_LOG_FRONTEND_LOG_HPP_INCLUDED
#define INITP_LOG_FRONTEND_LOG_HPP_INCLUDED

    // Do not include this header anywhere!

#include <initp/log/level.hpp>
#include <initp/log/frontend/config.pb.h>

#include <memory>
#include <string>

namespace initp {
namespace log {

class log_base {
public: // Construction
    log_base(level::value level);
    virtual ~log_base(void);
public: // Public methods
    virtual int type(void) const = 0;
    virtual void write_line(level::value level, const std::string& what, const std::string& error, const std::string& message) = 0;
public: // Static methods
    static void initialize(void);
public: // Factory methods
    static std::unique_ptr<log_base> create(const std_out_config& config);
    static std::unique_ptr<log_base> create(const char* app, const boost_log_config& config);
    static std::unique_ptr<log_base> create(const char* app, const http_initp_config& config);
    static std::unique_ptr<log_base> create(const char* app, const vector_config& config);
protected: // Protected fields
    level::value level_;
};

std::vector<std::unique_ptr<log_base>>& get_loggers(void);

}}

#endif // INITP_LOG_FRONTEND_LOG_HPP_INCLUDED
