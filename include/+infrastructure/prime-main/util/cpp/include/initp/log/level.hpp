#ifndef INITPLUS_LOG_LEVEL_HPP_INCLUDED
#define INITPLUS_LOG_LEVEL_HPP_INCLUDED

namespace initp {
namespace log {
namespace level {

enum value {
    LV_FATAL = 0,
    LV_ERROR,
    LV_WARNING,
    LV_INFO,
    LV_DEBUG,
    LV_TRACE
};

}

enum log_output_t {
    LO_STREAM = 1,
    LO_FILE = 2,
    LO_NET = 4,
    LO_ALL = 7
};

}}

#endif // INITPLUS_LOG_LEVEL_HPP_INCLUDED
