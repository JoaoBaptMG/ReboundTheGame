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

#include <cmath>
#include <glm/glm.hpp>

template <typename T>
T degreesToRadians(T degrees) { return T(0.0174532925199432957692369077) * degrees; }

template <typename T>
T radiansToDegrees(T radians) { return T(57.2957795130823208767981548141) * radians; }

template <typename T>
glm::tvec2<T> fromRadiusAngleRadians(T radius, T angle)
{
    return radius * glm::tvec2<T>{ std::cos(angle), std::sin(angle) };
}

template <typename T>
glm::tvec2<T> fromRadiusAngleDegrees(T radius, T angle)
{
    return fromRadiusAngleRadians(radius, degreesToRadians(angle));
}

template <typename T, glm::qualifier Q>
auto angle(const glm::vec<2, T, Q>& v)
{
    return glm::atan(v.y, v.x);
}

template <glm::length_t N, typename T, glm::qualifier Q>
auto angleBetween(const glm::vec<N,T,Q>& v1, const glm::vec<N, T, Q>& v2)
{
    return glm::acos(glm::dot(v1,v2)/(glm::length(v1)*glm::length(v2)));
}
