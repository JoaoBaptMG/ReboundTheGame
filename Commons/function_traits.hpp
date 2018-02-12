#pragma once

#include <cstdint>
#include <tuple>

namespace util
{
    template <typename F>
    struct function_traits {};
    
    template <typename R, typename... Args>
    struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)> {};

    template <typename R, typename... Args>
    struct function_traits<R(Args...)>
    {
        using return_type = R;

        static constexpr auto arity = sizeof...(Args);

        template <size_t N>
        struct argument
        {
            static_assert(N < arity, "error: invalid parameter index.");
            using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
        };
    };

    template<class C, class R, class... Args>
    struct function_traits<R(C::*)(Args...)> : public function_traits<R(C&,Args...)> {};
     
    template<class C, class R, class... Args>
    struct function_traits<R(C::*)(Args...) const> : public function_traits<R(const C&,Args...)> {};
}
