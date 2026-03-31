#ifndef INITPLUS_HTTP_SERVER_SESSION_HPP_INCLUDED
#define INITPLUS_HTTP_SERVER_SESSION_HPP_INCLUDED

#include <initp/system/time.hpp>

#include <cstddef>
#include <string>

namespace initp {
namespace http {

/**
 * \brief Cookie session data.
 */
class session_data {
    typedef session_data self_type;

public: // Construction
    session_data(void) = delete;
    session_data(const self_type&) = delete;
    session_data(self_type&&) = delete;
    session_data(const std::string&, system::time_t);
    ~session_data(void);

public: // Public methods
    void extend(system::time_t);
    void renew(system::time_t);

public: // Public properties
    const std::string& id(void) const;
    bool authorized(void) const;
    void authorized(bool);
    size_t connections(void) const;
    system::time_t time(void) const;

private: // Private fields
    std::string id_;
    bool authorized_;
    size_t connections_;
    system::time_t time_;
};

}}

#endif // INITPLUS_HTTP_SERVER_SESSION_HPP_INCLUDED
