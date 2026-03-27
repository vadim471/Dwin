#include <initp/utils/locale.hpp>
#include <initp/system/debug.hpp>

#include <boost/locale.hpp>

#include <locale>
#include <codecvt>

namespace initp {
namespace utils {
namespace locale {

std::string local_to_utf8(const std::string& string) {
    boost::locale::generator g;
    g.locale_cache_enabled(true);
    try {
        std::locale loc = g(boost::locale::util::get_system_locale());
        return boost::locale::conv::to_utf<char>(string, loc);
    } catch (std::exception& e) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::utils::locale::local_to_utf8", "Conversion failed. %s", e.what());
    }
    return std::string();
}

std::string utf8_to_local(const std::string& string) {
    boost::locale::generator g;
    g.locale_cache_enabled(true);
    try {
        std::locale loc = g(boost::locale::util::get_system_locale());
        return boost::locale::conv::from_utf<char>(string, loc);
    } catch (std::exception& e) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::utils::locale::utf8_to_local", "Conversion failed. %s", e.what());
    }
    return std::string();
}

std::string cp1251_to_utf8(const std::string& string) {
    boost::locale::generator g;
    g.locale_cache_enabled(true);
    try {
        std::locale loc = g.generate("ru_RU.CP1251");
        return boost::locale::conv::to_utf<char>(string, loc);
    } catch (std::exception& e) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::utils::locale::cp1251_to_utf8", "Conversion failed. %s", e.what());
    }
    return std::string();
}

std::string utf8_to_cp1251(const std::string& string) {
    boost::locale::generator g;
    g.locale_cache_enabled(true);
    try {
        std::locale loc = g.generate("ru_RU.CP1251");
        return boost::locale::conv::from_utf<char>(string, loc);
    } catch (std::exception& e) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "initp::utils::locale::utf8_to_cp1251", "Conversion failed. %s", e.what());
    }
    return std::string();
}

std::string utf16_to_utf8(const std::wstring& string) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(string);
}

std::wstring utf8_to_utf16(const std::string& string) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.from_bytes(string);
}

}}}
