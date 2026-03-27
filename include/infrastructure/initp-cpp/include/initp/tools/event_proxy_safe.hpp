#ifndef INITPLUS_TOOLS_EVENT_PROXY_SAFE_HPP_INCLUDED
#define INITPLUS_TOOLS_EVENT_PROXY_SAFE_HPP_INCLUDED

#include <memory>
#include <vector>
#include <functional>
#include <type_traits>

namespace initp {
namespace tools {

template<typename T>
class event_proxy_safe;

template<typename R, typename... Args>
class event_proxy_safe<R(*)(Args...)>:
    public event_proxy_safe<R(Args...)> {};

/**
 * \brief Event proxy for shared objects based on STL and variadic templates (C++11).
 */
template<typename R, typename... Args>
class event_proxy_safe<R(Args...)> {
private: // Private types
    typedef event_proxy_safe<R(Args...)> self_type;
public: // Public types
    typedef std::function<R(Args...)> handler_type;
    typedef std::shared_ptr<void> shared_type;
    typedef std::weak_ptr<void> object_type;
private: // Private types
    typedef std::pair<handler_type, object_type> value_type;
    typedef std::vector<value_type> vector;
public: // Construction
    event_proxy_safe(void) {}
    event_proxy_safe(const self_type& object):
        list_(object.list_) {}
    virtual ~event_proxy_safe(void) = default;
public: // Public methods
    void connect(const handler_type& f, shared_type object = NULL) {
        this->list_.push_back(std::make_pair(f, object_type(object)));
    }
    void remove(shared_type object) {
        if (!object) return;
        for (typename vector::iterator it = this->list_.begin(); it != this->list_.end();) {
            if (shared_type lock = it->second.lock()) {
                if (lock.get() == object.get()) {
                    it = this->list_.erase(it);
                } else ++it;
            } else {
                it = this->list_.erase(it);
            }
        }
    }
    void clear(void) {
        this->list_.clear();
    }
public: // Rise for all objects
    void rise(Args... args) {
        for (value_type& handler : this->list_) {
            if (shared_type lock = handler.second.lock()) {
                handler.first(args...);
            }
        }
    }
public: // Rise for specified object
    void rise_for(shared_type object, Args... args) {
        for (value_type& handler : this->list_) {
            if (shared_type lock = handler.second.lock()) {
                if (lock.get() == object.get()) {
                    handler.first(args...);
                }
            }
        }
    }
public: // Rise for all objects except specified
    void rise_except(shared_type object, Args... args) {
        for (value_type& handler : this->list_) {
            if (shared_type lock = handler.second.lock()) {
                if (lock.get() != object.get()) {
                    handler.first(args...);
                }
            }
        }
    }
private: // Private fields
    vector list_;
};

}}

#endif // INITPLUS_TOOLS_EVENT_PROXY_SAFE_HPP_INCLUDED
