#ifndef INITPLUS_TOOLS_STRING_HPP_INCLUDED
#define INITPLUS_TOOLS_STRING_HPP_INCLUDED

#include <string>
#include <sstream>
#include <iomanip>
#include <cstddef>
#include <cstdint>

namespace initp {
namespace tools {
namespace string {

template<typename T>
inline size_t strlen(const T* string) {
    size_t len;
    for (len = 0; string[len]; len++);
    return len;
}

template<typename T>
inline T* strcpy(const T* string) {
    size_t size = strlen(string) + 1;
    T* copy = new T[size];
    memcpy(copy, string, size * sizeof(T));
    return copy;
}

template<typename T>
inline bool strcmp(const T* s1, const T* s2) {
    size_t l1 = strlen<T>(s1);
    size_t l2 = strlen<T>(s2);
    if (l1 != l2) return false;
    for (size_t i = 0; i < l1; i++) {
        if (s1[i] != s2[i]) {
            return false;
        }
    }
    return true;
}

template<char REP = 0x20>
inline std::string wchar_to_string(const wchar_t* s) {
    size_t len = strlen(s);
    char* buf = new char[len + 1];
    for (size_t i = 0; i < len; i++) {
        if (s[i] < 0x100) {
            buf[i] = s[i];
        } else buf[i] = REP;
    }
    buf[len] = 0x0;
    std::string str(buf);
    delete[] buf;
    return str;
}

inline void trim_float(std::string& string) {
    while (!string.empty()) {
        if (string.back() == '0') {
            string.pop_back();
        } else if (string.back() == '.') {
            string.pop_back();
            break;
        } else break;
    }
}

template<typename T>
inline std::string to_string(const T& value, int precision = -1, bool fixed = false) {
    std::stringstream s;
    if (precision >= 0) {
        s << std::fixed << std::showpoint;
        s << std::setprecision(precision);
    }
    s << value;
    std::string string = s.str();
    if (precision >= 0 && !fixed)
        trim_float(string);
    return string;
}

template<typename T>
inline T parse(const std::string& string) {
    if (string.empty())
        return T(0);
    T value;
    std::stringstream stream;
    stream << string;
    stream >> value;
    return value;
}

template<typename T>
inline std::string to_string(const std::vector<T>& data) {
    if (data.empty()) return std::string("<empty>");
    std::stringstream stream;
    stream << std::setfill('0') << std::hex;
    size_t value_size = sizeof(T);
    if (value_size > 1) {
        stream << std::setw(value_size * 2) << *data.cbegin();
        for (auto it = data.cbegin() + 1; it != data.cend(); ++it) {
            stream << ' ' << std::setw(value_size * 2) << *it;
        }
    } else {
        stream << std::setw(value_size * 2) << (int64_t)(*data.cbegin());
        for (auto it = data.cbegin() + 1; it != data.cend(); ++it) {
            stream << ' ' << std::setw(value_size * 2) << (int64_t)(*it);
        }
    }
    return stream.str();
}

}}}

#endif // INITPLUS_TOOLS_STRING_HPP_INCLUDED
