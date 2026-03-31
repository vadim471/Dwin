#ifndef INITPLUS_HTTP_COOKIES_HPP_INCLUDED
#define INITPLUS_HTTP_COOKIES_HPP_INCLUDED

#include <unordered_map>
#include <string>

namespace initp {
namespace http {

/**
 * \brief Парсер HTTP cookies.
 */
class cookies {
public:
    typedef std::unordered_map<std::string, std::string> params_type;
public:
    cookies(const std::string&);
    cookies(const cookies&) = delete;
    virtual ~cookies(void) = default;
public:
    static std::string generate_session_id(void);
    static std::string make(const std::string& key, const std::string& value);
    static std::string make(const std::string& key, const std::string& value, const std::string& path);
public:
    params_type* operator->(void);
    params_type& params(void);
private:
    params_type params_;
};

}}

#endif // INITPLUS_HTTP_COOKIES_HPP_INCLUDED
