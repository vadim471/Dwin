#ifndef INITPLUS_TOOLS_FUNCTION_POOL_HPP_INCLUDED
#define INITPLUS_TOOLS_FUNCTION_POOL_HPP_INCLUDED

#include <functional>

#define DEFINE_FUNCTION_POOL(Z, N, T) template<> std::function<T> initp::tools::function_pool<N + 1, T>::handler = nullptr;

namespace initp {
namespace tools {

template<size_t SIZE, class F>
struct function_pool;

template<size_t SIZE, class R, class... Args>
struct function_pool<SIZE, R(*)(Args...)>:
    public function_pool<SIZE, R(Args...)> {};

template<size_t SIZE, class R, class... Args>
class function_pool<SIZE, R(Args...)> {
public:
    typedef R(type)(Args...);
    typedef std::function<R(Args...)> f_type;
public:
    static type* bind(const f_type& f) {
        if (!f) return nullptr;
        if (!handler) {
            handler = f;
            return &invoke;
        } else {
            return function_pool<SIZE - 1, R(Args...)>::bind(f);
        }
    }
    static void free(type* f) {
        if (&invoke != f) {
            return function_pool<SIZE - 1, R(Args...)>::free(f);
        } else {
            if (handler) handler = nullptr;
        }
    }
    static void clear(void) {
        if (handler) handler = nullptr;
        return function_pool<SIZE - 1, R(Args...)>::clear();
    }
    static R invoke(Args... args) {
        if (handler) {
            return handler(args...);
        } else return R();
    }
    static f_type handler;
};

template<class R, class... Args>
class function_pool<1, R(Args...)> {
public:
    typedef R(type)(Args...);
    typedef std::function<R(Args...)> f_type;
public:
    static type* bind(const f_type& f) {
        if (!f) return nullptr;
        if (!handler) {
            handler = f;
            return &invoke;
        } else {
            return nullptr;
        }
    }
    static void free(type* f) {
        if (&invoke != f) {
            return;
        } else {
            if (handler) handler = nullptr;
        }
    }
    static void clear(void) {
        if (handler) handler = nullptr;
    }
    static R invoke(Args... args) {
        if (handler) {
            return handler(args...);
        } else return R();
    }
    static f_type handler;
};

}}

#endif // INITPLUS_TOOLS_FUNCTION_POOL_HPP_INCLUDED
