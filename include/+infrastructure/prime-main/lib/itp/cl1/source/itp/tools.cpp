#include <itp/tools.hpp>
#include <itp/enum.h>

#include <boost/date_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <cstring>
#include <sstream>

namespace itp {
namespace tools {

uint8_t string_to_uint8(const char* string) {
    if (!string) return 0;
    uint16_t result = 0;
    for (uint8_t i = 0; i < 3; ++i) {
        if (!string[i]) break;
        if (string[i] < '0' || string[i] > '9')
            return 0;
        result *= 10;
        result += string[i] - '0';
    }
    if (result <= 0xFF) {
        return (uint8_t)result;
    } else return 0;
}

uint32_t string_to_uint32(const char* string) {
    if (!string) return 0;
    uint64_t result = 0;
    for (uint8_t i = 0; i < 9; ++i) {
        if (!string[i]) break;
        if (string[i] < '0' || string[i] > '9')
            return 0;
        result *= 10;
        result += string[i] - '0';
    }
    if (result <= 0xFFFFFFFF) {
        return (uint32_t)result;
    } else return 0;
}

bool string_to_bool(const char* string) {
    if (!string) return false;
    int length = strlen(string);
    if (length < 4 || length > 5) return false;
    if (strcmp(string, "true") == 0) {
        return true;
    } else if (strcmp(string, "false") == 0) {
        return false;
    } else return false;
}

std::string time_to_string(const std::string& prefix, uint64_t time) {
    //char array[] = "abcdefghijklmnopqrstuvwxyz";
    char array[] = "qazwsxedcrfvtgbyhnujmikolp1234567890";
    std::stringstream stream;
    boost::posix_time::ptime t = boost::posix_time::from_time_t(time / 1000L) + boost::posix_time::milliseconds(time % 1000L);
    boost::gregorian::date d = t.date();
    boost::posix_time::ptime epoch(boost::gregorian::date(d.year(), d.month(), d.day()));
    stream << prefix;
    stream << int(d.year() % 100);
    stream << array[d.month() - 1];
    stream << array[d.day() - 1];
    stream << (t - epoch).total_milliseconds();
    return stream.str();
}

uint8_t has_bit(int64_t* pack, uint8_t index) {
    return (*pack >> (index * 2)) & 0x1;
}

uint8_t get_bit(int64_t* pack, uint8_t index) {
    return (*pack >> (index * 2 + 1)) & 0x1;
}

void set_bit(int64_t* pack, uint8_t index, uint8_t value) {
    int64_t ONE_BIT = 0x1;
    *pack |= (ONE_BIT << (index * 2));
    if (value) {
        *pack |= (ONE_BIT << (index * 2 + 1));
    } else {
        *pack &= ~(ONE_BIT << (index * 2 + 1));
    }
}

}}
