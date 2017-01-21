#pragma once

namespace util
{
    class non_copyable
    {
        non_copyable(const non_copyable& other) = delete;
        non_copyable &operator=(const non_copyable& other) = delete;

    public:
        non_copyable() {}
    };

    class non_movable
    {
        non_movable(non_movable&& other) = delete;
        non_movable &operator=(non_movable&& other) = delete;

    public:
        non_movable() {}
    };

    class non_copyable_movable : non_copyable, non_movable {};
}
