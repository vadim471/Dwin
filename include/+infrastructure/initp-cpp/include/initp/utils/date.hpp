#ifndef INITPLUS_UTILS_DATE_HPP_INCLUDED
#define INITPLUS_UTILS_DATE_HPP_INCLUDED

#include <initp/system/time.hpp>

#include <string>

namespace initp {
namespace utils {
namespace date {

std::string format(system::time_t time, const char* format = "%d.%m.%Y %H:%M:%S");

system::time_t from_iso_extended_string(const std::string& date);

int64_t timezone_milliseconds(void);

}}}

#endif // INITPLUS_UTILS_DATE_HPP_INCLUDED
