#include <initp/system/debug.hpp>

#include <boost/algorithm/string.hpp>

namespace initp {
namespace system {

const std::string FATAL("fatal");
const std::string ERROR("error");
const std::string WARNING("warning");
const std::string INFO("info");
const std::string DEBUG("debug");
const std::string TRACE("trace");

uint8_t debug_level_to_integer(const char* level) {
    if (boost::iequals(level, FATAL)) {
        return SYSTEM_LEVEL_FATAL;
    } else if (boost::iequals(level, ERROR)) {
        return SYSTEM_LEVEL_ERROR;
    } else if (boost::iequals(level, WARNING)) {
        return SYSTEM_LEVEL_WARNING;
    } else if (boost::iequals(level, INFO)) {
        return SYSTEM_LEVEL_INFO;
    } else if (boost::iequals(level, DEBUG)) {
        return SYSTEM_LEVEL_DEBUG;
    } else if (boost::iequals(level, TRACE)) {
        return SYSTEM_LEVEL_TRACE;
    } else {
        return SYSTEM_LEVEL_TRACE;
    }
}

uint8_t debug_level_to_integer(const std::string& level) {
    return debug_level_to_integer(level.c_str());
}

std::string debug_level_to_string(uint8_t level, bool upper_case) {
    switch (level) {
        case SYSTEM_LEVEL_FATAL:
            return upper_case ? boost::algorithm::to_upper_copy(FATAL) : FATAL;
        case SYSTEM_LEVEL_ERROR:
            return upper_case ? boost::algorithm::to_upper_copy(ERROR) : ERROR;
        case SYSTEM_LEVEL_WARNING:
            return upper_case ? boost::algorithm::to_upper_copy(WARNING) : WARNING;
        case SYSTEM_LEVEL_INFO:
            return upper_case ? boost::algorithm::to_upper_copy(INFO) : INFO;
        case SYSTEM_LEVEL_DEBUG:
            return upper_case ? boost::algorithm::to_upper_copy(DEBUG) : DEBUG;
        case SYSTEM_LEVEL_TRACE:
        default:
            return upper_case ? boost::algorithm::to_upper_copy(TRACE) : TRACE;
    }
}

}}
