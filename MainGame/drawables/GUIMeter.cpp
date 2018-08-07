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


#include "GUIMeter.hpp"

constexpr float MeterWidthNormal = 36;
constexpr float MeterBorderNormal = 4;

constexpr float MeterWidthSmall = 14;
constexpr float MeterBorderSmall = 2;

GUIMeter::GUIMeter(MeterSize size, bool useCurrentAnimation) : size(size), useCurrentAnimation(useCurrentAnimation)
{
    float width = size == MeterSize::Normal ? MeterWidthNormal : MeterWidthSmall;

    setQuadColor(0, sf::Color::Black);
    setQuadColor(4, sf::Color(200, 200, 200));

    for (size_t i = 0; i < 16; i++)
        vertices[i].texCoords = sf::Vector2f(0, 0);

    vertices[16].texCoords = sf::Vector2f(0, 0);
    vertices[17].texCoords = sf::Vector2f(width, 0);
    vertices[18].texCoords = sf::Vector2f(width, width);
    vertices[19].texCoords = sf::Vector2f(0, width);

    updateVertices();
}

void GUIMeter::setRect(size_t ind, util::rect rect)
{
    vertices[ind].position = sf::Vector2f(rect.x, rect.y);
    vertices[ind+1].position = sf::Vector2f(rect.x + rect.width, rect.y);
    vertices[ind+2].position = sf::Vector2f(rect.x + rect.width, rect.y + rect.height);
    vertices[ind+3].position = sf::Vector2f(rect.x, rect.y + rect.height);
}

void GUIMeter::setQuadColor(size_t ind, sf::Color color)
{
    vertices[ind].color = color;
    vertices[ind+1].color = color;
    vertices[ind+2].color = color;
    vertices[ind+3].color = color;
}

void GUIMeter::update(FrameTime)
{
    if (current != target)
    {
        if (!useCurrentAnimation || current < target)
            current = target;
        else current--;

        updateVertices();
    }
}

void GUIMeter::updateVertices()
{
    float width = size == MeterSize::Normal ? MeterWidthNormal : MeterWidthSmall;
    float border = size == MeterSize::Normal ? MeterBorderNormal : MeterBorderSmall;

    float frameHeight = height + width + 3*border;
    float frameOffset = width + 2*border;
    setRect(0, util::rect(0, -frameHeight, width + 2*border, frameHeight));
    setRect(4, util::rect(border, -frameOffset-(float)height, width, (float)height));
    setRect(8, util::rect(border, -frameOffset-(float)current, width, (float)current));
    setRect(12, util::rect(border, -frameOffset-(float)target, width, (float)target));
    setRect(16, util::rect(border, -border-width, width, width));

    setQuadColor(4, backdropColor);
    setQuadColor(8, targetColor);
    setQuadColor(12, fillColor);
}

float GUIMeter::getFrameHeight() const
{
    float width = size == MeterSize::Normal ? MeterWidthNormal : MeterWidthSmall;
    float border = size == MeterSize::Normal ? MeterBorderNormal : MeterBorderSmall;
    return height + width + 3*border;
}

void GUIMeter::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform.translate(position.x, position.y);
    target.draw(vertices, 20, sf::Quads, states);
    
    if (icon)
    {
        bool wasSmooth = icon->isSmooth();
        if (!wasSmooth) icon->setSmooth(true);
        states.texture = icon.get();
        target.draw(vertices+16, 4, sf::Quads, states);
        if (wasSmooth) icon->setSmooth(false);
    }
}
