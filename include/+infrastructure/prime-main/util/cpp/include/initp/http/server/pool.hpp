#ifndef INITPLUS_HTTP_SERVER_POOL_HPP_INCLUDED
#define INITPLUS_HTTP_SERVER_POOL_HPP_INCLUDED

#include <initp/system/time.hpp>
#include <initp/http/server/session.hpp>

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>

namespace initp {
namespace http {

/**
 * \brief Pool for cookie session data.
 */
class session_pool {

public: // Construction
    session_pool(uint32_t, uint32_t, uint32_t);
    session_pool(const session_pool&) = delete;
    session_pool(session_pool&&) = delete;
    ~session_pool(void);

public: // Public methods
    void poll(system::time_t);
    std::string create(system::time_t);
    bool exist(const std::string&);
    bool check_and_extend(const std::string&);
    session_data* find(const std::string&);

private: // Private methods
    bool expired(const session_data&, system::time_t) const;

public: // Public properties
    void auth_timeout(uint32_t);
    void session_timeout(uint32_t);
    void max_sessions(uint32_t);

private: // Private fields
    std::unordered_map<std::string, session_data> map_;
    std::recursive_mutex mutex_;
    uint32_t auth_timeout_;
    uint32_t session_timeout_;
    uint32_t max_sessions_;
};

}}

#endif // INITPLUS_HTTP_SERVER_POOL_HPP_INCLUDED
