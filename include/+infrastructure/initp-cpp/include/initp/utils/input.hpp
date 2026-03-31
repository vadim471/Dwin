#ifndef INITPLUS_UTILS_INPUT_HPP_INCLUDED
#define INITPLUS_UTILS_INPUT_HPP_INCLUDED

#include <string>
#include <sstream>
#include <iostream>

namespace initp {
namespace utils {

template<typename T>
T input_value(const std::string& message = std::string(), const T& default_value = T()) {
    std::string buffer;
    if (!message.empty())
        std::cout << message << ": ";
    getline(std::cin, buffer);
    std::stringstream stream(buffer);
    T result(default_value);
    stream >> result;
    return result;
}

}}

#endif // INITPLUS_UTILS_INPUT_HPP_INCLUDED
