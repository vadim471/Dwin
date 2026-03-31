#include <initp/log/frontend/tools.hpp>

namespace initp {
namespace log {

level::value to_native_level(log_level level) {
    switch (level) {
        case log_level::LL_NULL:
        case log_level::LL_FATAL:
            return level::LV_FATAL;
        case log_level::LL_ERROR:
            return level::LV_ERROR;
        case log_level::LL_WARNING:
            return level::LV_WARNING;
        case log_level::LL_INFO:
            return level::LV_INFO;
        case log_level::LL_DEBUG:
            return level::LV_DEBUG;
        case log_level::LL_TRACE:
        case log_level::LL_ALL:
            return level::LV_TRACE;
        default:
            return level::LV_TRACE;
    }
}

}}
