#ifndef INITPLUS_SYSTEM_DEBUG_HPP_INCLUDED
#define INITPLUS_SYSTEM_DEBUG_HPP_INCLUDED

extern "C" {

#include <initp/system/debug.h>

}

#include <cstdint>
#include <string>

namespace initp {
namespace system {

uint8_t debug_level_to_integer(const char* level);
uint8_t debug_level_to_integer(const std::string& level);

std::string debug_level_to_string(uint8_t level, bool upper_case = false);

}}

#endif // INITPLUS_SYSTEM_DEBUG_HPP_INCLUDED
