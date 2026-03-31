#include <initp/http/cookies.hpp>
#include <initp/system/time.hpp>
#include <initp/utils/hash.hpp>

#include <sstream>
#include <algorithm>
#include <vector>

#include <boost/algorithm/string.hpp>

namespace initp {
namespace http {

std::string cookies::generate_session_id(void) {
    std::stringstream stream;
    stream << system::time::milliseconds();
    return utils::hash::md5(stream.str());
}

std::string cookies::make(const std::string& key, const std::string& value) {
    std::string data(key);
    data.append("=");
    data.append(value);
    return data;
}

std::string cookies::make(const std::string& key, const std::string& value, const std::string& path) {
    std::string data(key);
    data.append("=");
    data.append(value);
    data.append("; path=");
    data.append(path);
    return data;
}

cookies::cookies(const std::string& data):
    params_() {
    if (data.empty()) return;
    std::vector<std::string> params;
    boost::split(params, data, boost::is_any_of(";"), boost::token_compress_on);
    for (std::string param : params) {
        std::vector<std::string> pair;
        boost::split(pair, param, boost::is_any_of("="), boost::token_compress_on);
        if (pair.size() != 2) continue;
        if (pair.front().empty()) continue;
        boost::trim(pair.front());
        boost::trim(pair.back());
        boost::algorithm::to_lower(pair.front());
        //boost::algorithm::to_lower(pair.back());
        this->params_[pair.front()] = pair.back();
    }
}

cookies::params_type* cookies::operator->(void) {
    return &this->params_;
}

cookies::params_type& cookies::params(void) {
    return this->params_;
}

}}
