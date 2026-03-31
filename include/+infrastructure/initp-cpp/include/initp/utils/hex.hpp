#ifndef INITPLUS_UTILS_HEX_HPP_INCLUDED
#define INITPLUS_UTILS_HEX_HPP_INCLUDED

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <type_traits>

namespace initp {
namespace utils {
namespace hex {

template<typename T>
inline void decode(
    const std::string& input, std::vector<T>& output,
    typename std::enable_if<sizeof(T) == 1, void>::type* = NULL
) {
    uint32_t x;
    std::stringstream ss;
    ss << std::hex;
    size_t size = input.size() / 2;
    output.clear();
    output.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        ss.str(std::string());
        ss.clear();
        ss << input.substr(i * 2, 2);
        ss >> x;
        output.push_back(T(x));
    }
}

template<typename T>
inline void decode(
    const std::string& input, std::vector<T>& output,
    typename std::enable_if<sizeof(T) != 1, void>::type* = NULL
) {
    T x;
    std::stringstream ss;
    ss << std::hex;
    size_t t_size = sizeof(T) * 2;
    size_t size = input.size() / t_size;
    output.clear();
    output.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        ss.str(std::string());
        ss.clear();
        ss << input.substr(i * t_size, t_size);
        ss >> x;
        output.push_back(x);
    }
}

template<typename T>
inline std::string encode(const std::vector<T>& input) {
    std::stringstream ss;
    ss << std::hex;
    size_t type_size = sizeof(T);
    size_t value_size = type_size * 2;
    size_t data_size = input.size();
    for (size_t i = 0; i < data_size; ++i) {
        ss << std::setw(value_size) << std::setfill('0') << (int)input[i];
    }
    return ss.str();
}

}}}

#endif // INITPLUS_UTILS_HEX_HPP_INCLUDED
