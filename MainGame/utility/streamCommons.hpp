#pragma once

#include <SFML/System.hpp>
#include <type_traits>
#include <vector>
#include <string>
#include <utility>
#include <tuple>
#include "grid.hpp"

namespace util
{
    template <typename T, typename std::enable_if<std::is_pod<T>::value, int>::type = 0>
    bool readFromStream(sf::InputStream &stream, T& value)
    {
        return stream.read((void*)&value, sizeof(T)) == sizeof(T);
    }

    template <typename T>
    bool readFromStream(sf::InputStream &stream, std::basic_string<T> &value);

    template <typename T, typename std::enable_if<std::is_pod<T>::value, int>::type = 0>
    bool readFromStream(sf::InputStream &stream, std::vector<T> &value)
    {
        uint32_t size;

        if (!readFromStream(stream, size))
            return false;

        std::vector<T> newVal(size, T());
        if (stream.read(newVal.data(), size*sizeof(T)) != size*sizeof(T))
            return false;

        newVal.swap(value);
        return true;
    }

    template <typename T, typename std::enable_if<!std::is_pod<T>::value, int>::type = 0>
    bool readFromStream(sf::InputStream &stream, std::vector<T> &value)
    {
        uint32_t size;

        if (!readFromStream(stream, size))
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

    template <typename T, typename std::enable_if<std::is_standard_layout<T>::value, int>::type = 0>
    bool readFromStream(sf::InputStream &stream, grid<T> &value)
    {
        uint32_t width, height;

        if (!(readFromStream(stream, width) && readFromStream(stream, height)))
            return false;

        grid<T> newVal(width, height);

        auto size = width*height;
        if (stream.read(newVal.data(), size*sizeof(T)) != size*sizeof(T))
            return false;

        swap(newVal, value);
        return true;
    }

    template <typename T, typename std::enable_if<!std::is_standard_layout<T>::value, int>::type = 0>
    bool readFromStream(sf::InputStream &stream, grid<T> &value)
    {
        uint32_t width, height;

        if (!(readFromStream(stream, width) && readFromStream(stream, height)))
            return false;

        grid<T> newVal(width, height);
        for (auto &val : newVal)
            if (!readFromStream(stream, val))
                return false;

        newVal.swap(value);
        return true;
    }

    template <typename T1, typename T2>
    bool readFromStream(sf::InputStream& stream, std::pair<T1,T2>& pair)
    {
        return readFromStream(stream, pair.first) && readFromStream(stream, pair.second);
    }

    template <typename T, typename... Ts>
    bool readFromStream(sf::InputStream& stream, T& val, Ts&... nexts)
    {
        return readFromStream(stream, val) && readFromStream(stream, nexts...);
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
}
