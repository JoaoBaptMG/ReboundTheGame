#pragma once

#include <iostream>
#include <type_traits>
#include <vector>
#include <string>
#include <utility>
#include <tuple>
#include <unordered_map>
#include "VarLength.hpp"
#include "grid.hpp"

template <typename T, typename std::enable_if<std::is_standard_layout<T>::value, int>::type = 0>
bool writeToStream(std::ostream &stream, const T& value)
{
    return stream.write((const char*)&value, sizeof(T)).good();
}

static inline bool writeToStream(std::ostream &stream, const VarLength value)
{
    std::vector<uint8_t> chunks;
    auto val = value.target;

    do
    {
        chunks.push_back(val % 128);
        val /= 128;
    } while (val);

    for (size_t i = chunks.size(); i != 0; i--)
    {
        uint8_t b = chunks[i-1];
        if (i == 1) b += 128;
        if (!stream.put(b).good()) return false;
    }

    return true;
}

template <typename T>
bool writeToStream(std::ostream &stream, const std::basic_string<T> &value);

template <typename T, typename std::enable_if<is_optimization_viable<T>::value, int>::type = 0>
bool writeToStream(std::ostream &stream, const std::vector<T> &value)
{
    size_t size = value.size();

    if (!writeToStream(stream, VarLength(size)))
        return false;

    if (!stream.write((char*)value.data(), size*sizeof(T)).good())
        return false;
    
    return true;
}

template <typename T, typename std::enable_if<!is_optimization_viable<T>::value, int>::type = 0>
bool writeToStream(std::ostream &stream, const std::vector<T> &value)
{
    size_t size = value.size();

    if (!writeToStream(stream, VarLength(size)))
        return false;

    for (const auto &val : value)
        if (!writeToStream(stream, val))
            return false;
    
    return true;
}

template <typename T>
bool writeToStream(std::ostream &stream, const std::basic_string<T> &value)
{
    std::vector<T> val(value.begin(), value.end());
    
    if (!writeToStream(stream, val))
        return false;

    return true;
}

template <typename T, typename std::enable_if<is_optimization_viable<T>::value, int>::type = 0>
bool writeToStream(std::ostream &stream, const util::grid<T> &value)
{
    size_t width = value.width(), height = value.height();

    if (!(writeToStream(stream, VarLength(width)) && writeToStream(stream, VarLength(height))))
        return false;

    auto size = width*height;
    if (!stream.write((char*)value.data(), size*sizeof(T)).good())
        return false;

    return true;
}

template <typename T, typename std::enable_if<!is_optimization_viable<T>::value, int>::type = 0>
bool writeToStream(std::ostream &stream, const util::grid<T> &value)
{
    size_t width = value.width(), height = value.height();

    if (!(writeToStream(stream, VarLength(width)) && writeToStream(stream, VarLength(height))))
        return false;

    for (const auto &val : value)
        if (!writeToStream(stream, val)) return false;

    return true;
}

template <typename T, typename U>
bool writeToStream(std::ostream &stream, const std::unordered_map<T,U> &value)
{
    size_t size = value.size();
    if (!writeToStream(stream, VarLength(size)))
        return false;

    for (const auto& pair : value)
        if (!(writeToStream(stream, pair.first) && writeToStream(stream, pair.second)))
            return false;

    return true;
}

template <typename T, typename... Ts, typename std::enable_if_t<(sizeof...(Ts) > 0), int> = 0>
bool writeToStream(std::ostream& stream, T&& val, Ts&&... nexts)
{
    return writeToStream(stream, std::forward<T>(val)) && writeToStream(stream, std::forward<Ts>(nexts)...);
}

template <typename Tp, std::size_t... Is>
bool __wttsaux(std::ostream& stream, Tp& tuple, std::index_sequence<Is...>)
{
    return writeToStream(stream, std::get<Is>(tuple)...);
}

template <typename... Ts>
bool writeToStream(std::ostream& stream, std::tuple<Ts...>& tuple)
{
    return __wttsaux(stream, tuple, std::index_sequence_for<Ts...>());
}

