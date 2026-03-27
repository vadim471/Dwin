#ifndef INITPLUS_TOOLS_FUNCTION_TRAITS_HPP_INCLUDED
#define INITPLUS_TOOLS_FUNCTION_TRAITS_HPP_INCLUDED

#include <functional>
#include <tuple>

namespace initp {
namespace tools {

template<class F>
struct function_traits;

template<class R, class... Args>
struct function_traits<R(*)(Args...)>:
    public function_traits<R(Args...)> {};

template<typename... Args>
struct pack {};

template<class R, class... Args>
struct function_traits<R(Args...)> {
    using return_type = R;
    using arguments = pack<Args...>;
    static constexpr std::size_t arity = sizeof...(Args);
    template <std::size_t N>
    struct argument {
        static_assert(N < arity, "error: invalid parameter index.");
        using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };
};

}}

#endif // INITPLUS_TOOLS_FUNCTION_TRAITS_HPP_INCLUDED
