#ifndef INITPLUS_TOOLS_EVENT_PROXY_HPP_INCLUDED
#define INITPLUS_TOOLS_EVENT_PROXY_HPP_INCLUDED

#include <vector>
#include <functional>

namespace initp {
namespace tools {

template<typename F>
class event_proxy;

template<typename R, typename... Args>
class event_proxy<R(*)(Args...)>:
    public event_proxy<R(Args...)> {};

/**
 * \brief Event proxy based on STL and variadic templates (C++11).
 */
template<typename R, typename... Args>
class event_proxy<R(Args...)> {
private: // Private types
    typedef event_proxy<R(Args...)> self_type;
public: // Public types
    typedef std::function<R(Args...)> type;
private: // Private types
    typedef std::pair<type, void*> value_type;
    typedef std::vector<value_type> vector;
public: // Construction
    event_proxy(void) {}
    event_proxy(const self_type& object):
        list_(object.list_) {}
    virtual ~event_proxy(void) = default;
public: // Public methods
    void connect(const type& f, void* object = nullptr) {
        this->list_.push_back(std::make_pair(f, object));
    }
    void remove(void* object) {
        if (!object) return;
        for (typename vector::iterator it = this->list_.begin(); it != this->list_.end();) {
            if (it->second == object) {
                it = this->list_.erase(it);
            } else ++it;
        }
    }
    void clear(void) {
        this->list_.clear();
    }
public: // Rise for all objects
    void rise(Args... args) {
        for (value_type& handler : this->list_) {
            handler.first(std::forward<Args>(args)...);
        }
    }
public: // Rise for specified object
    void rise_for(void* object, Args... args) {
        for (value_type& handler : this->list_) {
            if (handler.second == object) {
                handler.first(std::forward<Args>(args)...);
            }
        }
    }
public: // Rise for all objects except specified
    void rise_except(void* object, Args... args) {
        for (value_type& handler : this->list_) {
            if (handler.second != object) {
                handler.first(std::forward<Args>(args)...);
            }
        }
    }
private: // Private fields
    vector list_;
};

}}

#endif // INITPLUS_TOOLS_EVENT_PROXY_HPP_INCLUDED
