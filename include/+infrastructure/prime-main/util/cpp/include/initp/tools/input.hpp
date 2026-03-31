#ifndef INITPLUS_TOOLS_INPUT_HPP_INCLUDED
#define INITPLUS_TOOLS_INPUT_HPP_INCLUDED

#include <string>
#include <sstream>
#include <iostream>

namespace initp {
namespace tools {
namespace string {

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

}}}

#endif // INITPLUS_TOOLS_INPUT_HPP_INCLUDED
