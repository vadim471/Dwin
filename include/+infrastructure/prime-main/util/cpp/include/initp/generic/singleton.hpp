#ifndef INITPLUS_GENERIC_SINGLETON_HPP_INCLUDED
#define INITPLUS_GENERIC_SINGLETON_HPP_INCLUDED

#include <initp/system/atexit.hpp>
#include <initp/system/compiler.hpp>

#include <atomic>
#include <new>
#include <utility>

namespace initp {
namespace tools {

template<typename T>
struct singleton_traits {
    static constexpr size_t priority = 0xFFFF;
};

namespace detail {
    void fill_with_trash(void* ptr, size_t len);

    void lock_recursive(std::atomic<size_t>& lock) noexcept;
    void unlock_recursive(std::atomic<size_t>& lock) noexcept;

    template<typename T>
    void destructor(void* ptr) {
        ((T*)ptr)->~T();
        fill_with_trash(ptr, sizeof(T));
    }

    template<typename T, size_t P, typename... TArgs>
    NO_INLINE T* singleton_base(std::atomic<T*>& ptr, TArgs&&... args) {
        alignas(T) static char buffer[sizeof(T)];
        static std::atomic<size_t> lock;

        lock_recursive(lock);

        auto ret = ptr.load();
        try {
            if (!ret) {
                ret = ::new (buffer) T(std::forward<TArgs>(args)...);
                try {
                    ::initp::system::at_exit(destructor<T>, ret, P);
                } catch (...) {
                    destructor<T>(ret);
                    throw;
                }
                ptr.store(ret);
            }
        } catch (...) {
            unlock_recursive(lock);
            throw;
        }

        unlock_recursive(lock);
        return ret;
    }

    template<typename T, size_t P, typename... TArgs>
    T* singleton_int(TArgs&&... args) {

        static std::atomic<T*> ptr;
        auto ret = ptr.load();

        if (unlikely(!ret)) {
            ret = singleton_base<T, P>(ptr, std::forward<TArgs>(args)...);
        }
        return ret;
    }

}

template <class T, class... TArgs>
RETURNS_NON_NULL T* singleton(TArgs&&... args) {
    return ::initp::tools::detail::singleton_int<T, singleton_traits<T>::priority>(std::forward<TArgs>(args)...);
}

template <class T, size_t P, class... TArgs>
RETURNS_NON_NULL T* singleton_with_priority(TArgs&&... args) {
    return ::initp::tools::detail::singleton_int<T, P>(std::forward<TArgs>(args)...);
}

}}

#endif // INITPLUS_GENERIC_SINGLETON_HPP_INCLUDED
