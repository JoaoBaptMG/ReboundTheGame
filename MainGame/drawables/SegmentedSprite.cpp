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


#include "SegmentedSprite.hpp"

#include <algorithm>
#include <cmath>

SegmentedSprite::SegmentedSprite(std::shared_ptr<sf::Texture> tex, glm::vec2 anchor)
    : Sprite(tex, anchor), centerRect()
{
    vertices.setPrimitiveType(sf::PrimitiveType::TriangleStrip);
    setupVertices();
}

SegmentedSprite::SegmentedSprite(std::shared_ptr<sf::Texture> tex)
    : SegmentedSprite(tex, glm::vec2(tex->getSize().x/2.0f, tex->getSize().y/2.0f)) {}
    
SegmentedSprite::SegmentedSprite() : SegmentedSprite(nullptr, glm::vec2(0, 0)) {}

util::rect SegmentedSprite::getBounds() const
{
    util::rect bounds(destRect);
    bounds.x -= getAnchorPoint().x;
    bounds.y -= getAnchorPoint().y;
    return bounds;
}

void SegmentedSprite::setupVertices()
{
    auto texRect = getTextureRect();
    
    if (!centerRect.intersects(texRect))
    {
        vertices.resize(0);
        return;
    }
    vertices.resize(22);
    for (size_t i = 0; i < vertices.getVertexCount(); i++)
        vertices[i].color = sf::Color::White;
        
    float texLeft = texRect.x, texTop = texRect.y;
    float texRight = texLeft + texRect.width, texBottom = texTop + texRect.height;
    float centerLeft = centerRect.x, centerTop = centerRect.y;
    float centerRight = centerLeft + centerRect.width, centerBottom = centerTop + centerRect.height;
    float destLeft = destRect.x, destTop = destRect.y;
    float destRight = destLeft + destRect.width, destBottom = destTop + destRect.height;
    
    float positionsX[] { destLeft, destLeft + centerLeft - texLeft, destRight - texRight + centerRight, destRight };
    float positionsY[] { destTop, destTop + centerTop - texTop, destBottom - texBottom + centerBottom, destBottom };
    
    float texturesX[] { texLeft, centerLeft, centerRight, texRight };
    float texturesY[] { texTop, centerTop, centerBottom, texBottom };
    
    vertices[0].position = sf::Vector2f(positionsX[0], positionsY[0]);
    vertices[0].texCoords = sf::Vector2f(texturesX[0], texturesY[0]);
    
    for (size_t j = 0; j < 3; j++)
        for (size_t i = 0; i < 7; i++)
        {
            size_t id = (i+1)/2;
            size_t jd = j+1 - (i%2);
            if (j == 1) id = 3 - id;
            
            vertices[7*j+i+1].position = sf::Vector2f(positionsX[id], positionsY[jd]);
            vertices[7*j+i+1].texCoords = sf::Vector2f(texturesX[id], texturesY[jd]);
        }
}
