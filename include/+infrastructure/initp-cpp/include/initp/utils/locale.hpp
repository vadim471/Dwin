#ifndef INITPLUS_UTILS_LOCALE_HPP_INCLUDED
#define INITPLUS_UTILS_LOCALE_HPP_INCLUDED

#include <string>

namespace initp {
namespace utils {
namespace locale {

std::string local_to_utf8(const std::string&);

std::string utf8_to_local(const std::string&);

std::string cp1251_to_utf8(const std::string&);

std::string utf8_to_cp1251(const std::string&);

std::string utf16_to_utf8(const std::wstring&);

std::wstring utf8_to_utf16(const std::string&);

}}}

#endif // INITPLUS_UTILS_LOCALE_HPP_INCLUDED
