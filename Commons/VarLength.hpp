#pragma once

struct VarLength
{
    size_t& target;
    VarLength(size_t& t) : target(t) {}
};

template <typename T>
struct is_optimization_viable : public std::is_trivial<T> {};
