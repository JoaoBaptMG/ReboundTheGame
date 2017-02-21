#pragma once

#include <chrono>

using FloatSeconds = std::chrono::duration<float>;
constexpr auto UpdateFrequency = std::chrono::milliseconds(16);

template <typename Ret, class Rep, class Period>
constexpr Ret toSeconds(std::chrono::duration<Rep,Period> duration)
{
    return std::chrono::duration_cast<std::chrono::duration<Ret>>(duration).count();
}
