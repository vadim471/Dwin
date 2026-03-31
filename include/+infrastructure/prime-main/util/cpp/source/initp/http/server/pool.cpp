#include <initp/http/server/pool.hpp>
#include <initp/http/cookies.hpp>
#include <initp/system/cdebug>
#include <initp/system/macro.hpp>

namespace initp {
namespace http {

    // Construction

session_pool::session_pool(uint32_t auth_timeout, uint32_t session_timeout, uint32_t max_sessions)
    : auth_timeout_(auth_timeout)
    , session_timeout_(session_timeout)
    , max_sessions_(max_sessions)
{}

session_pool::~session_pool(void) {}

    // Public methods

void session_pool::poll(system::time_t time) {
    LOCK_RMUTEX(this->mutex_);
    for (auto it = this->map_.begin(); it != this->map_.end();) {
        //if (it->second.expired(time)) {
        if (this->expired(it->second, time)) {
            sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::session_pool::poll", "Session %s expired", it->second.id().c_str());
            it = this->map_.erase(it);
            sys_debug_print(SYSTEM_LEVEL_DEBUG, "http::session_pool::poll", "Sessions remain: %d", this->map_.size());
        } else {
            ++it;
        }
    }
}

std::string session_pool::create(system::time_t time) {
    LOCK_RMUTEX(this->mutex_);
    if (this->map_.size() >= this->max_sessions_) {
        sys_debug_print(SYSTEM_LEVEL_WARNING, "http::session_pool::create", "Sessions pool overflow");
        return {};
    }
    std::string id = initp::http::cookies::generate_session_id();
    this->map_.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(id, time));
    sys_debug_print(SYSTEM_LEVEL_INFO, "http::session_pool::create", "Session %s created", id.c_str());
    sys_debug_print(SYSTEM_LEVEL_INFO, "http::session_pool::create", "Sessions now %d / %d", this->map_.size(), this->max_sessions_);
    return id;
}

bool session_pool::exist(const std::string& session_id) {
    LOCK_RMUTEX(this->mutex_);
    return this->map_.find(session_id) != this->map_.end();
}

bool session_pool::check_and_extend(const std::string& session_id) {
    LOCK_RMUTEX(this->mutex_);
    auto it = this->map_.find(session_id);
    if (it != this->map_.end()) {
        it->second.extend(500);
        return true;
    } else return false;
}

session_data* session_pool::find(const std::string& session_id) {
    auto it = this->map_.find(session_id);
    if (it != this->map_.end()) {
        return &it->second;
    }
    return nullptr;
}

    // Private methods

bool session_pool::expired(const session_data& session, system::time_t time) const {
    return session.time() + (session.authorized() ? this->session_timeout_ : this->auth_timeout_) < time;
}

    // Public properties

void session_pool::auth_timeout(uint32_t value) {
    this->auth_timeout_ = value;
}

void session_pool::session_timeout(uint32_t value) {
    this->session_timeout_ = value;
}

void session_pool::max_sessions(uint32_t value) {
    this->max_sessions_ = value;
}

}}
