#pragma once

#include <SFML/System.hpp>
#include <type_traits>
#include <vector>
#include <string>
#include <utility>
#include <tuple>
#include <unordered_map>
#include "VarLength.hpp"
#include "grid.hpp"

template <typename T, typename std::enable_if<std::is_standard_layout<T>::value, int>::type = 0>
bool readFromStream(sf::InputStream &stream, T& value)
{
    return stream.read((char*)&value, sizeof(T)) == sizeof(T);
}

static inline bool readFromStream(sf::InputStream &stream, VarLength value)
{
    value.target = 0;

    uint8_t curByte = 128;
    while (curByte & 128)
    {
        if (!readFromStream(stream, curByte)) return false;
        value.target = value.target * 128 + curByte % 128;
    }
    
    return true;
}

template <typename T>
bool readFromStream(sf::InputStream &stream, std::basic_string<T> &value);

template <typename T, typename std::enable_if<is_optimization_viable<T>::value, int>::type = 0>
bool readFromStream(sf::InputStream &stream, std::vector<T> &value)
{
    size_t size;

    if (!readFromStream(stream, VarLength(size)))
        return false;

    std::vector<T> newVal(size, T());
    if (stream.read((char*)newVal.data(), size*sizeof(T)) != size*sizeof(T))
        return false;

    newVal.swap(value);
    return true;
}

template <typename T, typename std::enable_if<!is_optimization_viable<T>::value, int>::type = 0>
bool readFromStream(sf::InputStream &stream, std::vector<T> &value)
{
    size_t size;

    if (!readFromStream(stream, VarLength(size)))
        return false;

    std::vector<T> newVal(size, T());
    for (auto &val : newVal)
        if (!readFromStream(stream, val))
            return false;

    newVal.swap(value);
    return true;
}

template <typename T>
bool readFromStream(sf::InputStream &stream, std::basic_string<T> &value)
{
    std::vector<T> val;
    if (!readFromStream(stream, val))
        return false;

    value.assign(val.begin(), val.end());
    return true;
}

template <typename T, typename std::enable_if<is_optimization_viable<T>::value, int>::type = 0>
bool readFromStream(sf::InputStream &stream, util::grid<T> &value)
{
    size_t width, height;

    if (!(readFromStream(stream, VarLength(width)) && readFromStream(stream, VarLength(height))))
        return false;

    util::grid<T> newVal(width, height);

    auto size = width*height;
    if (stream.read((char*)newVal.data(), size*sizeof(T)) != size*sizeof(T))
        return false;

    swap(newVal, value);
    return true;
}

template <typename T, typename std::enable_if<!is_optimization_viable<T>::value, int>::type = 0>
bool readFromStream(sf::InputStream &stream, util::grid<T> &value)
{
    size_t width, height;

    if (!(readFromStream(stream, VarLength(width)) && readFromStream(stream, VarLength(height))))
        return false;

    util::grid<T> newVal(width, height);
    for (auto &val : newVal)
        if (!readFromStream(stream, val))
            return false;

    swap(newVal, value);
    return true;
}

template <typename T, typename U>
bool readFromStream(sf::InputStream &stream, std::unordered_map<T,U> &value)
{
    std::unordered_map<T,U> newVal;

    size_t size;
    if (!readFromStream(stream, VarLength(size)))
        return false;

    for (size_t i = 0; i < size; i++)
    {
        T first; U second;
        if (!(readFromStream(stream, first) && readFromStream(stream, second)))
            return false;
        newVal.emplace(first, second);
    }

    swap(newVal, value);
    return true;
}

template <typename T, typename... Ts, typename std::enable_if_t<(sizeof...(Ts) > 0), int> = 0>
bool readFromStream(sf::InputStream& stream, T&& val, Ts&&... nexts)
{
    return readFromStream(stream, std::forward<T>(val)) && readFromStream(stream, std::forward<Ts>(nexts)...);
}

template <typename Tp, std::size_t... Is>
bool __rtfsaux(sf::InputStream& stream, Tp& tuple, std::index_sequence<Is...>)
{
    return readFromStream(stream, std::get<Is>(tuple)...);
}

template <typename... Ts>
bool readFromStream(sf::InputStream& stream, std::tuple<Ts...>& tuple)
{
    return __rtfsaux(stream, tuple, std::index_sequence_for<Ts...>());
}

inline static bool checkMagic(sf::InputStream& stream, const char *val)
{
    bool value = true;
    for (const char *p = val; *p; p++)
    {
        char c;
        if (!readFromStream(stream, c)) return false;
        if (c != *p) value = false;
    }
    return value;
}
