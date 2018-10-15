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

class Quad : public sf::Drawable
{
    sf::Color color;
    sf::FloatRect rect;
    
    sf::VertexArray vertices;
    void setupVertices();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
    Quad(sf::FloatRect rect, sf::Color color = sf::Color());
    virtual ~Quad() {}
    
    auto getRect() const { return rect; }
    void setRect(sf::FloatRect r) { rect = r; setupVertices(); }
    
    auto getColor() const { return color; }
    void setColor(sf::Color c) { color = c; setupVertices(); }
    
    auto getAlpha() const { return color.a; }
    void setAlpha(uint8_t a) { color.a = a; setupVertices(); }
};
