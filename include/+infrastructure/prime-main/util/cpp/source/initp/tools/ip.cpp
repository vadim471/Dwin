#include <initp/tools/ip.hpp>

#include <boost/algorithm/string.hpp>

#include <vector>
#include <sstream>

namespace initp {
namespace tools {
namespace ip {

std::tuple<std::string, std::string> split_address(const std::string& address, const std::string& default_port) {
    std::vector<std::string> data;
    boost::split(data, address, boost::is_any_of(":"), boost::token_compress_on);
    if (!data.empty()) {
        std::string host = data[0];
        std::string port = default_port;
        boost::trim_if(host, boost::is_any_of(" "));
        if (data.size() > 1) {
            port = data[1];
            boost::trim_if(port, boost::is_any_of(" "));
        }
        return std::make_tuple(host, port);
    }
    return std::make_tuple(std::string(), std::string());
}

std::tuple<std::string, uint16_t> split_address(const std::string& address, uint16_t default_port) {
    std::vector<std::string> data;
    boost::split(data, address, boost::is_any_of(":"), boost::token_compress_on);
    if (!data.empty()) {
        std::string host = data[0];
        uint16_t port = default_port;
        boost::trim_if(host, boost::is_any_of(" "));
        if (data.size() > 1) {
            std::string string = data[1];
            boost::trim_if(string, boost::is_any_of(" "));
            std::stringstream stream;
            stream << string;
            uint64_t value = 0;
            stream >> value;
            if (value <= 0xFFFF) {
                port = (uint16_t)value;
            }
        }
        return std::make_tuple(host, port);
    }
    return std::make_tuple(std::string(), uint16_t(0));
}

}}}
