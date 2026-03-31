#ifndef INITPLUS_HTTP_HEADER_LIST_HPP_INCLUDED
#define INITPLUS_HTTP_HEADER_LIST_HPP_INCLUDED

#include <boost/beast/http/field.hpp>

#include <string>
#include <vector>

namespace initp {
namespace http {

class header_list {
public:
    typedef std::vector<std::pair<std::string, std::string>> list_type;
public:
    header_list(void);
public:

    std::string& operator[](boost::beast::http::field key);
    std::string& operator[](const std::string& key);

    list_type& list(void);
    const list_type& list(void) const;

    list_type::iterator begin(void);
    list_type::const_iterator begin(void) const;
    list_type::const_iterator cbegin(void) const;

    list_type::iterator end(void);
    list_type::const_iterator end(void) const;
    list_type::const_iterator cend(void) const;

    list_type::const_iterator find(boost::beast::http::field key) const;
    list_type::const_iterator find(const std::string& key) const;

    list_type::size_type size(void) const;
private:
    list_type list_;
};

}}

#endif // INITPLUS_HTTP_HEADER_LIST_HPP_INCLUDED
