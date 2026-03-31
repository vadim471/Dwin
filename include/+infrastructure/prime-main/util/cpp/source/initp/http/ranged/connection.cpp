#include <initp/http/ranged/connection.hpp>

#include <initp/system/debug.hpp>
#include <initp/tools/string.hpp>

#include <boost/algorithm/string.hpp>

#include <sstream>

namespace initp {
namespace http {
namespace ranged {

    // Static methods

system::error_code connection::parse_range(const std::string& range, size_t& first, size_t& last, size_t& total) {

    if (range.empty()) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::connection::parse_range", "Empty string");
        return system::err::bad_argument;
    }

    std::vector<std::string> chunks;
    boost::split(chunks, range, boost::is_any_of(" -/"), boost::token_compress_on);
    if (chunks.size() != 4) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::connection::parse_range", "Bad range: %s", range.c_str());
        return system::err::bad_data;
    }

    first = tools::string::parse<size_t>(chunks[1]);
    last = tools::string::parse<size_t>(chunks[2]);
    total = tools::string::parse<size_t>(chunks[3]);
    return system::err::success;
}

std::string connection::get_next_range(size_t total, size_t max, const std::string& range, size_t& first, size_t& last) {

    if (!total || !max) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "http::ranged::connection::get_next_range", "Bad arguments");
        return std::string();
    }

    size_t begin = 0, end = 0;

    if (range.empty()) {
        first = last = 0;
        begin = 0;
        end = max - 1;
        if (end >= total) {
            end = total - 1;
        }
    } else {
        size_t total_range;
        if (parse_range(range, first, last, total_range)) return std::string();
        if (total != total_range) {
            sys_debug_print(SYSTEM_LEVEL_WARNING, "http::ranged::connection::get_next_range", "Totals does not match: %d != %d", total, total_range);
        }
        begin = last + 1;
        if (begin >= total) return std::string();
        end = begin + max - 1;
        if (end >= total) {
            end = total - 1;
        }
    }

    std::stringstream stream;
    stream << "bytes=" << begin << "-" << end;
    return stream.str();
}

}}}
