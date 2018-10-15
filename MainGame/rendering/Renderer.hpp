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

#include "Drawable.hpp"
#include <memory>
#include <map>
#include <stack>
#include <non_copyable_movable.hpp>
#include <glm/glm.hpp>
#include <transforms.hpp>

using RenderData = std::pair<std::reference_wrapper<Drawable>,glm::mat3>;

class Renderer final : util::non_copyable
{
    std::multimap<long,RenderData> drawableList;
    std::stack<glm::mat3> transformStack;

public:
	Renderer() noexcept;
    Renderer(Renderer&& other) noexcept;
    Renderer& operator=(Renderer other);

    void pushDrawable(Drawable& drawable, long depth = 0);

    void pushTransform();
    void popTransform();

    void render();
    void clearState();

	glm::mat3 currentTransform;
    
    friend void swap(Renderer& r1, Renderer& r2);
};

