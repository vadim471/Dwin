#ifndef INITPLUS_GENERIC_FUNCTION_GENERIC_HPP_INCLUDED
#define INITPLUS_GENERIC_FUNCTION_GENERIC_HPP_INCLUDED

#include <functional>
#include <vector>
#include <tuple>

#include <initp/system/cdebug>

namespace initp {
namespace tools {

namespace detail {

class wrapper {
    typedef wrapper self_type;
public:
    wrapper(void) {}
    wrapper(const self_type&) = delete;
    virtual ~wrapper(void) {}
public:
    static void free(size_t address) {
        if (address) delete (self_type*)address;
    }
};

template<typename F>
class wrapper_impl:
    public virtual wrapper {
    typedef wrapper_impl<F> self_type;
public:
    wrapper_impl(const std::function<F>& f):
        wrapper(), f_(f) {}
    std::function<F> f_;
};

}

template<class F = void(void)>
struct function;

template<class R, class... Args>
class function<std::function<R(Args...)>>:
    public function<R(Args...)> {};

template<class R, class... Args>
struct function<R(*)(Args...)>:
    public function<R(Args...)> {};

template<class R, class... Args>
class function<R(Args...)> {
    typedef function<R(Args...)> self_type;
public:
    typedef R(r_type)(size_t, Args...);
    typedef R(f_type)(Args...);
public:
    static std::tuple<r_type*, size_t> bind(const std::function<f_type>& f) {
        size_t object = (size_t)new detail::wrapper_impl<f_type>(f);
        //sys_debug_print(SYSTEM_LEVEL_DEBUG, "tools::function::bind", "Bind address 0x%x", object);
        return std::make_tuple(&call, object);
    }
    static void free(size_t address) {
        if (address) delete (detail::wrapper*)address;
    }
private:
    static R call(size_t address, Args... args) {
        if (!address) {
            sys_debug_print(SYSTEM_LEVEL_FATAL, "tools::function::call", "Call null pointer");
            return R();
        }
        return ((detail::wrapper_impl<f_type>*)address)->f_(args...);
    }
};

template<class F = void(size_t)>
struct function_pointer;

template<class R, class... Args>
struct function_pointer<R(*)(size_t, Args...)>:
    public function_pointer<R(size_t, Args...)> {};

template<class R, class... Args>
struct function_pointer<R(size_t, Args...)> {
    static void free(size_t address) {
        delete (detail::wrapper_impl<R(Args...)>*)address;
    }
};

}}

#endif // INITPLUS_GENERIC_FUNCTION_GENERIC_HPP_INCLUDED
