#pragma once

#include <cmath>
#include <SFML/Graphics.hpp>

template <typename T>
T degreesToRadians(T degrees) { return T(0.0174532925199432957692369077) * degrees; }

template <typename T>
T radiansToDegrees(T radians) { return T(57.2957795130823208767981548141) * radians; }

template <typename T>
T dot(const sf::Vector2<T>& v1, const sf::Vector2<T>& v2)
{
    return v1.x*v2.x + v1.y*v2.y;
}

template <typename T>
T dot(const sf::Vector3<T>& v1, const sf::Vector3<T>& v2)
{
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

template <typename T>
auto lengthSquared(const T& v) { return dot(v,v); }

template <typename T>
auto length(const T& v) { return std::sqrt(lengthSquared(v)); }

template <typename T>
auto normalize(const T& v) { return v/length(v); }

template <typename T>
auto distanceSquared(const T& v1, const T& v2) { return lengthSquared(v1-v2); }

template <typename T>
auto distance(const T& v1, const T& v2) { return std::sqrt(distanceSquared(v1,v2)); }

template <typename T>
auto project(const T& v, const T& vp)
{
    return vp * dot(v,vp)/dot(vp,vp);
}

template <typename T>
T cross(const sf::Vector2<T>& v1, const sf::Vector2<T>& v2)
{
    return v1.x*v2.y - v1.y*v2.x;
}

template <typename T>
sf::Vector3<T> cross(const sf::Vector3<T>& v1, const sf::Vector3<T>& v2)
{
    return { v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x };
}

template <typename T>
sf::Vector2<T> fromRadiusAngleRadians(T radius, T angle)
{
    return radius * sf::Vector2<T>{ std::cos(angle), std::sin(angle) };
}

template <typename T>
sf::Vector2<T> fromRadiusAngleDegrees(T radius, T angle)
{
    return fromRadiusAngleRadians(radius, degreesToRadians(angle));
}

template <typename T>
auto angle(const sf::Vector2<T>& v)
{
    return std::atan2(v.y, v.x);
}

template <typename T>
auto angleBetween(const T& v1, const T& v2)
{
    return std::acos(dot(v1,v2)/std::sqrt(lengthSquared(v1)*lengthSquared(v2)));
}
