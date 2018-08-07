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

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <rect.hpp>
#include <cmath>

class ScissorRectGuard
{
    GLint prevScissor[4];
    GLboolean prevScissorStatus;

public:
    ScissorRectGuard();
    ScissorRectGuard(const util::rect& rect);
    ~ScissorRectGuard();
};

class ScissorRectPush : public sf::Drawable
{
    util::rect scissorRect;

public:
    ScissorRectPush() : scissorRect(NAN, NAN, NAN, NAN) {}
    ScissorRectPush(util::rect rect) : scissorRect(rect) {}

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

public:
    friend struct ScissorRect;
};

class ScissorRectPop : public sf::Drawable
{
    ScissorRectPop() {}
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

public:
    friend struct ScissorRect;
};

struct ScissorRect
{
    static const ScissorRectPop& pop();
    static const ScissorRectPush& push();
};
