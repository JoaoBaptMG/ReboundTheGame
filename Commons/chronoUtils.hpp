#pragma once

#include <chrono>

using Sixtieth = std::ratio<1, 60>;
struct FrameClock
{
    using rep = int64_t;
    using period = Sixtieth;
    using duration = std::chrono::duration<rep, period>;
    using time_point = std::chrono::time_point<FrameClock>;
    static const bool is_steady = std::chrono::steady_clock::is_steady;

    static time_point now() noexcept
    {
        auto nowDur = std::chrono::steady_clock::now().time_since_epoch();
        auto frameDur = std::chrono::duration_cast<duration>(nowDur);
        return time_point(frameDur);
    }
};
using FrameTime = FrameClock::time_point;
using FrameDuration = FrameClock::duration;

inline static constexpr FrameDuration operator ""_frames(unsigned long long val)
{
    return FrameDuration(val);
}

using FloatSeconds = std::chrono::duration<float>;
constexpr auto UpdatePeriod = FrameDuration(1);

template <typename Ret, class Rep, class Period>
constexpr Ret toSeconds(std::chrono::duration<Rep,Period> duration)
{
    return std::chrono::duration_cast<std::chrono::duration<Ret>>(duration).count();
}

template <typename T, typename = std::enable_if_t<std::is_default_constructible<T>::value>>
void reset(T& val) { val = T(); }

template <typename T, typename = std::enable_if_t<std::is_default_constructible<T>::value>>
bool isNull(const T& val) { return val == T(); }
