#ifndef INITPLUS_GENERIC_WEAK_BIND_HPP_INCLUDED
#define INITPLUS_GENERIC_WEAK_BIND_HPP_INCLUDED

#ifdef SHOW_BIND_CLASS_NAME
#include <initp/generic/enable_class_name.hpp>
#include <initp/system/cdebug>
#endif // SHOW_BIND_CLASS_NAME

#include "function_traits.hpp"

#include <memory>
#include <functional>
#include <type_traits>

namespace initp {
namespace tools {

class weak {
public:
    template<typename F, typename T>
    static std::function<F> bind(const std::function<F>& f, std::shared_ptr<T> ptr,
        typename std::enable_if<function_traits<F>::arity == 0, int>::type* = NULL) {
        return std::bind(&weak::invoker<T,
            typename function_traits<F>::return_type
        >, f, std::weak_ptr<T>(ptr));
    }
    template<typename F, typename T>
    static std::function<F> bind(const std::function<F>& f, std::shared_ptr<T> ptr,
        typename std::enable_if<function_traits<F>::arity == 1, int>::type* = NULL) {
        return std::bind(&weak::invoker<T,
            typename function_traits<F>::return_type,
            typename function_traits<F>::template argument<0>::type
        >, f, std::weak_ptr<T>(ptr), std::placeholders::_1);
    }
    template<typename F, typename T>
    static std::function<F> bind(const std::function<F>& f, std::shared_ptr<T> ptr,
        typename std::enable_if<function_traits<F>::arity == 2, int>::type* = NULL) {
        return std::bind(&weak::invoker<T,
            typename function_traits<F>::return_type,
            typename function_traits<F>::template argument<0>::type,
            typename function_traits<F>::template argument<1>::type
        >, f, std::weak_ptr<T>(ptr), std::placeholders::_1, std::placeholders::_2);
    }
    template<typename F, typename T>
    static std::function<F> bind(const std::function<F>& f, std::shared_ptr<T> ptr,
        typename std::enable_if<function_traits<F>::arity == 3, int>::type* = NULL) {
        return std::bind(&weak::invoker<T,
            typename function_traits<F>::return_type,
            typename function_traits<F>::template argument<0>::type,
            typename function_traits<F>::template argument<1>::type,
            typename function_traits<F>::template argument<2>::type
        >, f, std::weak_ptr<T>(ptr), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }
protected:
    template<typename T, typename R, typename... Args>
    static R invoker(
        const std::function<R(Args...)>& f,
        std::weak_ptr<T> wptr, Args... args
    ) {
        if (std::shared_ptr<T> lock = wptr.lock()) {
            return f(args...);
        } else {
            #ifdef SHOW_BIND_CLASS_NAME
            std::string name = utils::get_class_name<T>();
            sys_debug_print(SYSTEM_LEVEL_WARNING, "tools::weak::invoker", "Object is gone for class %s", name);
            #endif // SHOW_BIND_CLASS_NAME
        }
    }
};

}}

#endif // INITPLUS_GENERIC_WEAK_BIND_HPP_INCLUDED
