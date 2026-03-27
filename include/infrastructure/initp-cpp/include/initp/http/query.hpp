#ifndef INITPLUS_HTTP_QUERY_HPP_INCLUDED
#define INITPLUS_HTTP_QUERY_HPP_INCLUDED

#include <unordered_map>
#include <vector>
#include <string>

namespace initp {
namespace http {

/**
 * \brief Парсер строки запроса URI.
 */
class query {

public: // Public types
    typedef std::vector<std::string> path_type;
    typedef std::unordered_map<std::string, std::string> params_type;

public: // Static functions
    static std::string encode(const std::string&);
    static std::string decode(const std::string&);

public: // Construction
    query(void);
    query(const std::string& uri);
    query(const query&) = delete;
    virtual ~query(void) = default;

public: // Public methods
    void parse(const std::string& uri);
    query& append(const std::string& path);
    query& add(const std::string& key, const std::string& value);
    std::string dump(void) const;
    std::string dump_path(void) const;
    std::string dump_params(void) const;

public: // Public properties
    path_type& path(void);
    params_type& params(void);
    bool escape(void) const;
    void escape(bool);

private: // Private fields
    path_type path_;
    params_type params_;
    bool escape_;
};

}}

#endif // INITPLUS_HTTP_QUERY_HPP_INCLUDED
