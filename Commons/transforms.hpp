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

#include <glm/glm.hpp>
#include <cmath>

namespace util
{
	static glm::mat3 identity(1.0f);

	inline static glm::mat3 translate(glm::vec2 v)
	{
		return glm::mat3(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(v.x, v.y, 1));
	}

	inline static glm::mat3 translate(float x, float y) { return translate(glm::vec2(x, y)); }

	inline static glm::mat3 rotate(float t)
	{
		auto c = std::cos(t), s = std::sin(t);
		return glm::mat3(glm::vec3(c, s, 0), glm::vec3(-s, c, 0), glm::vec3(0, 0, 1));
	}

	inline static glm::mat3 rotateDegrees(float t) { return rotate(t * M_PI / 180); }

	inline static glm::mat3 scale(glm::vec2 v)
	{
		return glm::mat3(glm::vec3(v.x, 0, 0), glm::vec3(0, v.y, 0), glm::vec3(0, 0, 1));
	}

	inline static glm::mat3 scale(float x, float y) { return scale(glm::vec2(x, y)); }
	inline static glm::mat3 scale(float s) { return scale(s, s); }
}