#ifndef INITPLUS_TOOLS_FUNCTION_HPP_INCLUDED
#define INITPLUS_TOOLS_FUNCTION_HPP_INCLUDED

#include <functional>
#include <vector>
#include <tuple>

namespace initp {
namespace tools {

template<class F>
struct function;

template<class R, class... Args>
struct function<R(*)(Args...)>:
    public function<R(Args...)> {};

template<class R, class... Args>
class function<R(Args...)> {
    typedef function<R(Args...)> self_type;
public:
    typedef R(r_type)(size_t, Args...);
    typedef std::function<R(Args...)> f_type;
private:
    typedef std::tuple<size_t, f_type, bool> value_type;
    typedef std::vector<value_type> vector;
public:
    static std::tuple<r_type*, size_t> bind(f_type f, bool call_once = true) {
        size_t index = ++index_;
        list_.emplace_back(std::make_tuple(index, f, call_once));
        return std::make_tuple(&call, index);
    }
    static void erase(size_t index) {
        for (typename vector::iterator it = list_.begin(); it != list_.end(); ++it) {
            if (std::get<0>(*it) == index) {
                list_.erase(it);
                return;
            }
        }
    }
    static void clear(void) {
        list_.clear();
        index_ = 0;
    }
private:
    static R call(size_t index, Args... args) {
        for (typename vector::iterator it = list_.begin(); it != list_.end(); ++it) {
            if (std::get<0>(*it) == index) {
                if (std::get<2>(*it)) {
                    f_type f = std::get<1>(*it);
                    list_.erase(it);
                    return f(args...);
                } else return std::get<1>(*it)(args...);
            }
        }
        return R();
    }
private:
    static size_t index_;
    static vector list_;
};

template<class R, class... Args>
size_t function<R(Args...)>::index_ = 0;
template<class R, class... Args>
typename function<R(Args...)>::vector function<R(Args...)>::list_;

}}

#endif // INITPLUS_TOOLS_FUNCTION_HPP_INCLUDED
