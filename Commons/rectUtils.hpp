#pragma once

#include <SFML/System.hpp>
#include <algorithm>
#include <cmath>

template <typename T>
sf::Rect<T> rectUnionWithPoint(const sf::Rect<T> &rect, const sf::Vector2<T> &point)
{
    auto left = std::isnan(rect.left) ? point.x : std::min(rect.left, point.x);
    auto top = std::isnan(rect.top) ? point.y : std::min(rect.top, point.y);
    auto right = std::isnan(rect.left) || std::isnan(rect.width) ? point.x : std::max(rect.left + rect.width, point.x);
    auto bottom = std::isnan(rect.top) || std::isnan(rect.height) ? point.y : std::max(rect.top + rect.height, point.y);
    
    return sf::Rect<T>(left, top, right - left, bottom - top);
}

template <typename T>
sf::Rect<T> rectUnionWithLineX(const sf::Rect<T> &rect, const T& x)
{
    auto left = std::isnan(rect.left) ? x : std::min(rect.left, x);
    auto right = std::isnan(rect.left) || std::isnan(rect.width) ? x : std::max(rect.left + rect.width, x);
    
    return sf::Rect<T>(left, rect.top, right - left, rect.height);
}

template <typename T>
sf::Rect<T> rectUnionWithRect(const sf::Rect<T> &r1, const sf::Rect<T> &r2)
{
    auto left = std::min(r1.left, r2.left);
    auto top = std::min(r1.top, r2.top);
    auto right = std::max(r1.left + r1.width, r2.left + r2.width);
    auto bottom = std::max(r1.top + r1.height, r2.top + r2.height);
    
    return sf::Rect<T>(left, top, right - left, bottom - top);
}

template <typename T>
sf::Rect<T> rectIntersectionWithRect(const sf::Rect<T> &r1, const sf::Rect<T> &r2)
{
    auto left = std::max(r1.left, r2.left);
    auto top = std::max(r1.top, r2.top);
    auto right = std::min(r1.left + r1.width, r2.left + r2.width);
    auto bottom = std::min(r1.top + r1.height, r2.top + r2.height);
    
    return sf::Rect<T>(left, top, right - left, bottom - top);
}
