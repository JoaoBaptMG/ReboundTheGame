//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//


#pragma once

#include "OutputStream.hpp"
#include <type_traits>
#include <vector>
#include <string>
#include <utility>
#include <tuple>
#include <unordered_map>
#include "VarLength.hpp"
#include "grid.hpp"

template <typename T, typename std::enable_if<std::is_standard_layout<T>::value, int>::type = 0>
bool writeToStream(OutputStream &stream, const T& value)
{
    return stream.write(&value, sizeof(T));
}

static inline bool writeToStream(OutputStream &stream, const VarLength value)
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
        if (i > 1) b += 128;
        if (!stream.write(&b, sizeof(uint8_t))) return false;
    }

    return true;
}

template <typename T>
bool writeToStream(OutputStream &stream, const std::basic_string<T> &value);

template <typename T, typename std::enable_if<is_optimization_viable<T>::value, int>::type = 0>
bool writeToStream(OutputStream &stream, const std::vector<T> &value)
{
    if (!writeToStream(stream, varLength(value.size())))
        return false;
    if (value.size() == 0) return true;
    if (!stream.write(value.data(), value.size()*sizeof(T)))
        return false;
    
    return true;
}

template <typename T, typename std::enable_if<!is_optimization_viable<T>::value, int>::type = 0>
bool writeToStream(OutputStream &stream, const std::vector<T> &value)
{
    if (!writeToStream(stream, varLength(value.size())))
        return false;

    for (const auto &val : value)
        if (!writeToStream(stream, val))
            return false;
    
    return true;
}

template <typename T>
bool writeToStream(OutputStream &stream, const std::basic_string<T> &value)
{
    std::vector<T> val(value.begin(), value.end());
    
    if (!writeToStream(stream, val))
        return false;

    return true;
}

template <typename T, typename std::enable_if<is_optimization_viable<T>::value, int>::type = 0>
bool writeToStream(OutputStream &stream, const util::grid<T> &value)
{
    if (!(writeToStream(stream, varLength(value.width())) && writeToStream(stream, varLength(value.height()))))
        return false;

    auto size = value.width()*value.height();
    if (size == 0) return true;
    if (!stream.write(value.data(), size*sizeof(T)))
        return false;

    return true;
}

template <typename T, typename std::enable_if<!is_optimization_viable<T>::value, int>::type = 0>
bool writeToStream(OutputStream &stream, const util::grid<T> &value)
{
    if (!(writeToStream(stream, varLength(value.width())) && writeToStream(stream, varLength(value.height()))))
        return false;

    for (const auto &val : value)
        if (!writeToStream(stream, val)) return false;

    return true;
}

template <typename T, typename U>
bool writeToStream(OutputStream &stream, const std::unordered_map<T,U> &value)
{
    if (!writeToStream(stream, varLength(value.size())))
        return false;

    for (const auto& pair : value)
        if (!(writeToStream(stream, pair.first) && writeToStream(stream, pair.second)))
            return false;

    return true;
}

template <typename T, typename... Ts, typename std::enable_if_t<(sizeof...(Ts) > 0), int> = 0>
bool writeToStream(OutputStream& stream, T&& val, Ts&&... nexts)
{
    return writeToStream(stream, std::forward<T>(val)) && writeToStream(stream, std::forward<Ts>(nexts)...);
}

template <typename Tp, std::size_t... Is>
bool __wttsaux(OutputStream& stream, Tp& tuple, std::index_sequence<Is...>)
{
    return writeToStream(stream, std::get<Is>(tuple)...);
}

template <typename... Ts>
bool writeToStream(OutputStream& stream, std::tuple<Ts...>& tuple)
{
    return __wttsaux(stream, tuple, std::index_sequence_for<Ts...>());
}

inline static bool writeMagic(OutputStream& stream, const char *val)
{
    for (const char *p = val; *p; p++)
        if (!writeToStream(stream, *p)) return false;
    return true;
}
