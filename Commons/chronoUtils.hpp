#pragma once

#include <chrono>

using FloatSeconds = std::chrono::duration<float>;
constexpr auto UpdateFrequency = std::chrono::microseconds(1000000)/60;

template <typename Ret, class Rep, class Period>
constexpr Ret toSeconds(std::chrono::duration<Rep,Period> duration)
{
    return std::chrono::duration_cast<std::chrono::duration<Ret>>(duration).count();
}

template <typename T, typename = std::enable_if_t<std::is_default_constructible<T>::value>>
void reset(T& val) { val = T(); }

template <typename T, typename = std::enable_if_t<std::is_default_constructible<T>::value>>
bool isNull(const T& val) { return val == T(); }
