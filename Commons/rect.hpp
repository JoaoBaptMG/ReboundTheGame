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
#include <algorithm>
#include <cmath>

// TODO: remove me
#include <SFML/Graphics.hpp>

namespace util
{
	template <typename T>
	struct basic_rect
	{
		T x, y, width, height;

		basic_rect() : x(), y(), width(), height() {}
		basic_rect(T x, T y, T w, T h) : x(x), y(y), width(w), height(h)
		{
			if (width < T()) { width = -width; x -= width; }
			if (height < T()) { height = -height; y -= height; }
		}

		basic_rect(const sf::Rect<T>& o) : basic_rect(o.left, o.top, o.width, o.height) {}
		
		template <glm::qualifier Q>
		basic_rect(const glm::vec<2, T, Q>& pos, const glm::vec<2, T, Q>& size) : x(pos.x), y(pos.y), width(size.x), height(size.y) {}

		static basic_rect fromTwoPoints(T x1, T y1, T x2, T y2) { return basic_rect(x1, y1, x2 - x1, y2 - y1); }
		template <glm::qualifier Q>
		static basic_rect fromTwoPoints(const glm::vec<2, T, Q>& p1, const glm::vec<2, T, Q>& p2) { return basic_rect(p1, p2 - p1); }

		bool contains(T xp, T yp) const
		{
			if (xp < x || xp >= x + width) return false;
			if (yp < y || yp >= y + height) return false;
			return true;
		}

		template <glm::qualifier Q>
		bool contains(const glm::vec<2, T, Q>& p) const { return contains(p.x, p.y); }

		bool intersects(const basic_rect<T>& o)
		{
			if (o.x + o.width <= x || x + width <= o.x) return false;
			if (o.y + o.height <= y || y + height <= o.y) return false;
			return true;
		}

		template <glm::qualifier Q>
		basic_rect unite(const glm::vec<2,T,Q> &p) const
		{
			auto x1 = std::isnan(x) ? p.x : std::min(x, p.x);
			auto y1 = std::isnan(y) ? p.y : std::min(y, p.y);
			auto x2 = std::isnan(x) || std::isnan(width) ? p.x : std::max(x + width, p.x);
			auto y2 = std::isnan(y) || std::isnan(height) ? p.y : std::max(y + height, p.y);

			return basic_rect(x1, y1, x2 - x1, y2 - y1);
		}

		basic_rect unionWithLineX(const T& xl) const
		{
			auto l = std::isnan(x) ? xl : std::min(x, xl);
			auto r = std::isnan(x) || std::isnan(width) ? xl : std::max(x + width, xl);

			return basic_rect(l, y, r - l, height);
		}

		basic_rect unite(const basic_rect &o) const
		{
			auto left = std::min(x, o.x);
			auto top = std::min(y, o.y);
			auto right = std::max(x + width, o.x + o.width);
			auto bottom = std::max(y + height, o.y + o.height);

			return basic_rect(left, top, right - left, bottom - top);
		}

		basic_rect intersect(const basic_rect &o) const
		{
			auto left = std::max(x, o.x);
			auto top = std::max(y, o.y);
			auto right = std::min(x + width, o.x + o.width);
			auto bottom = std::min(y + height, o.y + o.height);

			return basic_rect(left, top, right - left, bottom - top);
		}

		operator sf::Rect<T>() const { return sf::Rect<T>(x, y, width, height); }
	};

	using rect = basic_rect<float>;
	using drect = basic_rect<double>;
	using irect = basic_rect<int>;
	using urect = basic_rect<unsigned>;
}

template <typename T, glm::qualifier Q>
inline util::basic_rect<T> operator*(glm::mat<3,3,T,Q> m, util::basic_rect<T> r)
{
	auto v1 = m * glm::vec<3,T,Q>(r.x, r.y, 1.0f);
	auto v2 = m * glm::vec<3,T,Q>(r.x + r.width, r.y, 1.0f);
	auto v3 = m * glm::vec<3,T,Q>(r.x + r.width, r.y + r.height, 1.0f);
	auto v4 = m * glm::vec<3,T,Q>(r.x, r.y + r.height, 1.0f);

	auto x1 = std::min({ v1.x, v2.x, v3.x, v4.x });
	auto x2 = std::max({ v1.x, v2.x, v3.x, v4.x });
	auto y1 = std::min({ v1.y, v2.y, v3.y, v4.y });
	auto y2 = std::max({ v1.y, v2.y, v3.y, v4.y });

	return util::basic_rect<T>::fromTwoPoints(x1, y1, x2, y2);
}
