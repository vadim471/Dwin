#include <initp/http/server/session.hpp>

namespace initp {
namespace http {

    // Construction

session_data::session_data(const std::string& id, system::time_t time)
    : id_(id)
    , authorized_(false)
    , connections_(0)
    , time_(time)
{}

session_data::~session_data(void) {}

    // Public methods

void session_data::extend(system::time_t time) {
    this->time_ += time;
}

void session_data::renew(system::time_t time) {
    this->time_ = time;
}

    // Public properties

const std::string& session_data::id(void) const {
    return this->id_;
}

bool session_data::authorized(void) const {
    return this->authorized_;
}

void session_data::authorized(bool value) {
    this->authorized_ = value;
}

size_t session_data::connections(void) const {
    return this->connections_;
}

system::time_t session_data::time(void) const {
    return this->time_;
}

}}
