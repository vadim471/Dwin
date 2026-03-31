#ifndef INITPLUS_LOG_FRONTEND_LOGGER_HPP_INCLUDED
#define INITPLUS_LOG_FRONTEND_LOGGER_HPP_INCLUDED

#include <initp/log/level.hpp>

#include <sstream>

namespace initp {
namespace log {

class frontend_config;

class log_helper : public std::stringstream {
    template<typename T, typename E> friend log_helper fatal(const T&, const E&, int);
    template<typename T, typename E> friend log_helper error(const T&, const E&, int);
    template<typename T, typename E> friend log_helper warning(const T&, const E&, int);
    template<typename T> friend log_helper debug(const T&, int);
    template<typename T> friend log_helper info(const T&, int);
    template<typename T> friend log_helper trace(const T&, int);
public: // Construction
    log_helper(level::value, int);
    log_helper(log_helper&&);
    virtual ~log_helper();
public: // Public properties
    std::stringstream& what(void);
    std::stringstream& error(void);
private: // Private fields
    bool skip_message_;
    level::value level_;
    int output_mask_;
    std::stringstream what_;
    std::stringstream error_;
};

template<typename T>
T* find(void);

    // Setup functions

void setup(void);
void setup(const char* app, const frontend_config& config);

    // Log functions

template<typename T, typename E>
log_helper fatal(const T& what, const E& err, int mask) {
    log_helper helper(level::LV_FATAL, mask);
    helper.what_ << what;
    helper.error_ << err;
    return helper;
}
template<typename T, typename E>
log_helper fatal(const T& what, const E& err) {
    return fatal(what, err, LO_ALL);
}

template<typename T, typename E>
log_helper error(const T& what, const E& err, int mask) {
    log_helper helper(level::LV_ERROR, mask);
    helper.what_ << what;
    helper.error_ << err;
    return helper;
}
template<typename T, typename E>
log_helper error(const T& what, const E& err) {
    return error(what, err, LO_ALL);
}

template<typename T, typename E>
log_helper warning(const T& what, const E& err, int mask) {
    log_helper helper(level::LV_WARNING, mask);
    helper.what_ << what;
    helper.error_ << err;
    return helper;
}
template<typename T, typename E>
log_helper warning(const T& what, const E& err) {
    return warning(what, err, LO_ALL);
}

template<typename T>
log_helper debug(const T& what, int mask) {
    log_helper helper(level::LV_DEBUG, mask);
    helper.what_ << what;
    return helper;
}
template<typename T>
log_helper debug(const T& what) {
    return debug(what, LO_ALL);
}

template<typename T>
log_helper info(const T& what, int mask) {
    log_helper helper(level::LV_INFO, mask);
    helper.what_ << what;
    return helper;
}
template<typename T>
log_helper info(const T& what) {
    return info(what, LO_ALL);
}

template<typename T>
log_helper trace(const T& what, int mask) {
    log_helper helper(level::LV_TRACE, mask);
    helper.what_ << what;
    return helper;
}
template<typename T>
log_helper trace(const T& what) {
    return trace(what, LO_ALL);
}

}}

#endif // INITPLUS_LOG_FRONTEND_LOGGER_HPP_INCLUDED
