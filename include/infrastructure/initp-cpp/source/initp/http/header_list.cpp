#include <initp/http/header_list.hpp>

#include <boost/algorithm/string.hpp>

namespace initp {
namespace http {

header_list::header_list(void) {}

std::string& header_list::operator[](boost::beast::http::field key) {
    return this->operator[](boost::beast::http::to_string(key).to_string());
}

std::string& header_list::operator[](const std::string& key) {
    for (auto& p : this->list_)
        if (boost::iequals(p.first, key))
            return std::ref(p.second);
    this->list_.emplace_back(key, std::string());
    return std::ref(this->list_.back().second);
}

header_list::list_type& header_list::list(void) {
    return this->list_;
}

const header_list::list_type& header_list::list(void) const {
    return this->list_;
}

header_list::list_type::iterator header_list::begin(void) {
    return this->list_.begin();
}

header_list::list_type::const_iterator header_list::begin(void) const {
    return this->list_.cbegin();
}

header_list::list_type::const_iterator header_list::cbegin(void) const {
    return this->list_.cbegin();
}

header_list::list_type::iterator header_list::end(void) {
    return this->list_.end();
}

header_list::list_type::const_iterator header_list::end(void) const {
    return this->list_.cend();
}

header_list::list_type::const_iterator header_list::cend(void) const {
    return this->list_.cend();
}

header_list::list_type::const_iterator header_list::find(boost::beast::http::field key) const {
    std::string string = boost::beast::http::to_string(key).to_string();
    for (list_type::const_iterator it = this->list_.cbegin(); it != this->list_.cend(); ++it)
        if (!std::get<0>(*it).compare(string)) return it;
    return this->list_.cend();
}

header_list::list_type::const_iterator header_list::find(const std::string& key) const {
    for (list_type::const_iterator it = this->list_.cbegin(); it != this->list_.cend(); ++it)
        if (!std::get<0>(*it).compare(key)) return it;
    return this->list_.cend();
}

header_list::list_type::size_type header_list::size(void) const {
    return this->list_.size();
}

}}
