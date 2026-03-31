#include <initp/system/atexit.hpp>

#include <boost/thread/locks.hpp>
#include <boost/thread/reverse_lock.hpp>

#include <atomic>
#include <queue>
#include <deque>
#include <tuple>
#include <mutex>

#include <cassert>
#include <cstdlib>
#include <cstdint>

namespace initp {
namespace system {

namespace {
    class at_exit_wrapper {

    private: // Internal structures
        struct f_type {
            at_exit_ctx_f f;
            void* context;
            size_t priority;
            size_t number;
        };

        struct comparator {
            inline bool operator()(const f_type* l, const f_type* r) const noexcept {
                return std::tie(l->priority, l->number) < std::tie(r->priority, r->number);
            }
        };

    public: // Construction
        inline at_exit_wrapper(void) noexcept
            : finish_called_(false)
        {}

    public: // Public methods
        inline void finish(void) noexcept {
            this->finish_called_.store(true);

            boost::unique_lock<std::mutex> lock(this->lock_);

            while (!this->items_.empty()) {

                auto c = this->items_.top();
                assert(c);
                this->items_.pop();

                {
                    boost::reverse_lock<boost::unique_lock<std::mutex>> unlock(lock);
                    try {
                        c->f(c->context);
                    } catch (...) {
                        // Shit happens
                    }
                }
            }
        }

        inline void push(at_exit_ctx_f f, void* context, size_t priority) {
            boost::unique_lock<std::mutex> lock(this->lock_);
            this->store_.push_back({ f, context, priority, this->store_.size() });
            this->items_.push(&this->store_.back());
        }

    public: // Public properties
        inline bool finish_called(void) const {
            return this->finish_called_.load();
        }

    private: // Private fields
        std::mutex lock_;
        std::atomic<bool> finish_called_;
        std::deque<f_type> store_;
        std::priority_queue<f_type*, std::vector<f_type*>, comparator> items_;
    };

    static std::mutex at_exit_lock;
    static std::atomic<at_exit_wrapper*> at_exit_ptr = nullptr;
    alignas(at_exit_wrapper) static char at_exit_memory[sizeof(at_exit_wrapper)];

    static void on_exit(void) {
        if (at_exit_wrapper* const wrapper = at_exit_ptr.load()) {
            wrapper->finish();
            wrapper->~at_exit_wrapper();
            at_exit_ptr.store(nullptr);
        }
    }

    static inline at_exit_wrapper* instance() {
        if (at_exit_wrapper* const wrapper = at_exit_ptr.load(std::memory_order_acquire)) {
            return wrapper;
        }
        boost::unique_lock<std::mutex> lock(at_exit_lock);
        if (at_exit_wrapper* const wrapper = at_exit_ptr.load()) {
            return wrapper;
        }
        std::atexit(on_exit);
        at_exit_wrapper* const wrapper = new (at_exit_memory) at_exit_wrapper;
        at_exit_ptr.store(wrapper, std::memory_order_release);
        return wrapper;
    }
}

bool is_exit_started(void) {
    if (at_exit_wrapper* const wrapper = at_exit_ptr.load(std::memory_order_acquire)) {
        return wrapper->finish_called();
    }
    return false;
}

void at_exit(at_exit_ctx_f f, void* context, size_t priority) {
    instance()->push(f, context, priority);
}

void at_exit(at_exit_ctx_f f, void* context) {
    at_exit(f, context, SIZE_MAX);
}

static void close_no_context(void* context) {
    reinterpret_cast<at_exit_f>(context)();
}

void at_exit(at_exit_f f) {
    at_exit(close_no_context, reinterpret_cast<void*>(f));
}

void at_exit(at_exit_f f, size_t priority) {
    at_exit(close_no_context, reinterpret_cast<void*>(f), priority);
}

}}
