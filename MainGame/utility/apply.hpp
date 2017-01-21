#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace detail
{
    template<typename F, typename Tuple, std::size_t ...S>
    decltype(auto) apply_impl(F&& fn, Tuple&& t, std::index_sequence<S...>)
    {
        return std::forward<F>(fn)(std::get<S>(std::forward<Tuple>(t))...);
    }
}

template<typename F, typename Tuple>
decltype(auto) apply(F&& fn, Tuple&& t)
{
	std::size_t constexpr tSize = std::tuple_size<typename std::remove_reference<Tuple>::type>::value;
	return detail::apply_impl(std::forward<F>(fn), std::forward<Tuple>(t), std::make_index_sequence<tSize>());
}
