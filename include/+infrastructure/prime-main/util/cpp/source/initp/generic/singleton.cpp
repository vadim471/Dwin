#include <initp/generic/singleton.hpp>
#include <initp/platform/tools.hpp>
#include <initp/system/time.hpp>

#include <atomic>
#include <cassert>
#include <cstring>
#include <thread>

namespace initp {
namespace tools {

namespace {
    static inline bool atomic_try_lock(std::atomic<size_t>& a, size_t v) noexcept {
        size_t zero = 0;
        return a.compare_exchange_strong(zero, v);
    }

    static inline bool atomic_try_and_try_lock(std::atomic<size_t>& a, size_t v) noexcept {
        return a.load(std::memory_order_acquire) == 0 && atomic_try_lock(a, v);
    }
}

void detail::fill_with_trash(void* ptr, size_t length) {
    memset(ptr, 0xBA, length);
}

void detail::lock_recursive(std::atomic<size_t>& lock) noexcept {
    const size_t id = system::tools::current_thread_id();

    assert(lock.load(std::memory_order_acquire) != id && "Recursive singleton initialization");

    if (!atomic_try_lock(lock, id)) {
        do {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        } while (!atomic_try_and_try_lock(lock, id));
    }
}

void detail::unlock_recursive(std::atomic<size_t>& lock) noexcept {
    assert(lock.load(std::memory_order_acquire) == system::tools::current_thread_id() && "Unlock from other thread?");
    lock.store(0);
}

}}
