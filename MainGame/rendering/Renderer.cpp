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


#include "Renderer.hpp"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Renderer::Renderer(Renderer&& other) noexcept : Renderer()
{
    swap(*this, other);
}
    
Renderer& Renderer::operator=(Renderer other)
{
    swap(*this, other);
    return *this;
}

void swap(Renderer& r1, Renderer& r2)
{
    using std::swap;
    
    swap(r1.drawableList, r2.drawableList);
    swap(r1.transformStack, r2.transformStack);
    swap(r1.currentTransform, r2.currentTransform);
}

inline static std::ostream& operator<<(std::ostream& out, const glm::mat3& transform)
{
	auto mtx = glm::value_ptr(transform);
    out << '(' << mtx[0];
    for (size_t i = 1; i < 9; i++) out << ',' << mtx[i];
    return out << ')';
}

void Renderer::pushDrawable(const sf::Drawable &drawable, sf::RenderStates states, long depth)
{
	sf::Transform transformedTransform
	(
		currentTransform[0][0], currentTransform[1][0], currentTransform[2][0],
		currentTransform[0][1], currentTransform[1][1], currentTransform[2][1],
		currentTransform[0][2], currentTransform[1][2], currentTransform[2][2]
	);
    states.transform.combine(transformedTransform);
    drawableList.emplace(depth, std::make_pair(std::cref(drawable), states));
}

void Renderer::render(sf::RenderTarget& target)
{
    for (const auto& pair : drawableList)
        target.draw(pair.second.first, pair.second.second);
}

void Renderer::clearState()
{
    drawableList.clear();

    while (!transformStack.empty())
        transformStack.pop();

	currentTransform = util::identity;
}

void Renderer::pushTransform()
{
    transformStack.push(currentTransform);
}

void Renderer::popTransform()
{
    currentTransform = transformStack.top();
    transformStack.pop();
}
