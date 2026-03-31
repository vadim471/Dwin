#include <initp/http/query.hpp>

#include <boost/algorithm/string.hpp>

#include <cstdio>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace initp {
namespace http {

    // Static functions

std::string query::encode(const std::string& value) {

    std::stringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {

        std::string::value_type c = *i;

        // Keep alphanumeric and other accepted characters intact
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char)c);
        escaped << std::nouppercase;
    }
    return escaped.str();
}

std::string query::decode(const std::string& value) {

    std::stringstream stream;
    unsigned int decoded;

    for (size_t i = 0, len = value.length(); i < len; ++i) {
        if (value[i] != '%') {
            if (value[i] == '+') {
                stream << ' ';
            } else {
                stream << value[i];
            }
        } else {
            sscanf(value.substr(i + 1, 2).c_str(), "%x", &decoded);
            stream << static_cast<char>(decoded);
            i = i + 2;
        }
    }
    return stream.str();
}

    // Construction

query::query(void):
    path_(),
    params_(),
    escape_(true)
{}

query::query(const std::string& target):
    path_(),
    params_(),
    escape_(true)
{
    this->parse(target);
}

    // Public methods

void query::parse(const std::string& target) {
    if (target.empty()) return;
    std::string::const_iterator it = std::find(target.cbegin(), target.cend(), '?');
    std::string path(target.cbegin(), it);
    boost::trim_if(path, boost::is_any_of("\\/"));
    if (!path.empty())
        boost::split(this->path_, path, boost::is_any_of("\\/"));
    if (it != target.cend()) {
        std::string query(it + 1, target.cend());
        boost::trim_if(query, boost::is_any_of("&"));
        std::vector<std::string> params;
        if (!query.empty())
            boost::split(params, query, boost::is_any_of("&"), boost::token_compress_on);
        for (std::string param : params) {
            std::vector<std::string> pair;
            boost::split(pair, param, boost::is_any_of("="), boost::token_compress_on);
            if (pair.size() != 2) continue;
            if (pair.front().empty()) continue;
            boost::algorithm::to_lower(pair.front());
            this->params_[pair.front()] = decode(pair.back());
        }
    }
}

query& query::append(const std::string& path) {
    this->path_.push_back(path);
    return *this;
}

query& query::add(const std::string& key, const std::string& value) {
    this->params_[key] = this->escape_ ? encode(value) : value;
    return *this;
}

std::string query::dump(void) const {
    std::stringstream s;
    s << this->dump_path() << "?" << this->dump_params();
    return s.str();
}

std::string query::dump_path(void) const {
    std::stringstream s;
    for (auto const& p : this->path_)
        s << "/" << p;
    return s.str();
}

std::string query::dump_params(void) const {
    std::stringstream s;
    for (auto const& p : this->params_) {
        if (s.rdbuf()->in_avail()) s << "&";
        s << p.first << "=" << p.second;
    }
    return s.str();
}

    // Public properties

query::path_type& query::path(void) {
    return this->path_;
}

query::params_type& query::params(void) {
    return this->params_;
}

bool query::escape(void) const {
    return this->escape_;
}

void query::escape(bool value) {
    this->escape_ = value;
}

}}
