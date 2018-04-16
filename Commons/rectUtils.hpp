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
