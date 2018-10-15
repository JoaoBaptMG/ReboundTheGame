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

#include "Quad.hpp"

#include <assert.hpp>

Quad::Quad(util::rect rect, glm::u8vec4 color) : rect(rect), color(color), vertices(sf::PrimitiveType::TriangleFan)
{
    setupVertices();
}

void Quad::setupVertices()
{
    vertices.resize(4);
    
    vertices[0] = sf::Vertex(sf::Vector2f(rect.x, rect.y), color);
    vertices[1] = sf::Vertex(sf::Vector2f(rect.x + rect.width, rect.y), color);
    vertices[2] = sf::Vertex(sf::Vector2f(rect.x + rect.width, rect.y + rect.height), color);
    vertices[3] = sf::Vertex(sf::Vector2f(rect.x, rect.y + rect.height), color);
}

void Quad::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(vertices, states);
}
