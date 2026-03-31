#ifndef INITPLUS_GTK_DISPATCHER_HPP_INCLUDED
#define INITPLUS_GTK_DISPATCHER_HPP_INCLUDED

#include "macro.hpp"
#include "../tools/dispatch.hpp"

#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <functional>

#include <glibmm/dispatcher.h>

namespace initp {
namespace gtk {

class dispatcher:
    public virtual initp::tools::dispatcher {
private: // Private types
    typedef dispatcher self_type;
public: // Public types
    typedef std::function<void(void)> function;
public: // Construction
    dispatcher(void):
        dispatcher_(),
        mutex_(),
        queue_() {
        this->dispatcher_.connect(SIGC(on_dispatch));
    }
    dispatcher(const self_type&) = delete;
    ~dispatcher(void) {}
public: // Public methods
    virtual void dispatch(const function& f) {
        if (!this->is_thread_owned()) {
            {
                std::lock_guard<std::mutex> lock(this->mutex_);
                this->queue_.push(f);
            }
            this->dispatcher_.emit();
        } else {
            f();
        }
    }
    void push(const function& f) {
        if (!this->is_thread_owned()) {
            std::lock_guard<std::mutex> lock(this->mutex_);
            this->queue_.push(f);
        } else f();
    }
    void emit(void) {
        this->dispatcher_.emit();
    }
private: // Private methods
    void on_dispatch(void) {
        for (;;) {
            this->mutex_.lock();
            if (this->queue_.empty()) {
                this->mutex_.unlock();
                return;
            }
            function& f = this->queue_.front();
            this->mutex_.unlock();
            f();
            {
                std::lock_guard<std::mutex> lock(this->mutex_);
                this->queue_.pop();
            }
        }
    }
public: // Public properties
    bool is_thread_owned(void) {
        return this->id_ == std::this_thread::get_id();
    }
    const std::thread::id& id(void) {
        return this->id_;
    }
    void id(const std::thread::id& id) {
        this->id_ = id;
    }
private: // Private fields
    std::thread::id id_;
    Glib::Dispatcher dispatcher_;
    std::mutex mutex_;
    std::queue<function> queue_;
};

}}

#endif // INITPLUS_GTK_DISPATCHER_HPP_INCLUDED
