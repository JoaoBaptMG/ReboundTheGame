#include "SegmentedSprite.hpp"

#include <algorithm>
#include <cmath>

SegmentedSprite::SegmentedSprite(std::shared_ptr<sf::Texture> tex, sf::Vector2f anchor)
    : Sprite(tex, anchor), centerRect()
{
    vertices.setPrimitiveType(sf::PrimitiveType::TriangleStrip);
    setupVertices();
}

SegmentedSprite::SegmentedSprite(std::shared_ptr<sf::Texture> tex)
    : SegmentedSprite(tex, sf::Vector2f(tex->getSize())/2.0f) {}
    
SegmentedSprite::SegmentedSprite() : SegmentedSprite(nullptr, sf::Vector2f(0, 0)) {}

sf::FloatRect SegmentedSprite::getBounds() const
{
    sf::FloatRect bounds(destRect);
    bounds.left -= getAnchorPoint().x;
    bounds.top -= getAnchorPoint().y;
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
        
    float texLeft = texRect.left, texTop = texRect.top;
    float texRight = texLeft + texRect.width, texBottom = texTop + texRect.height;
    float centerLeft = centerRect.left, centerTop = centerRect.top;
    float centerRight = centerLeft + centerRect.width, centerBottom = centerTop + centerRect.height;
    float destLeft = destRect.left, destTop = destRect.top;
    float destRight = destLeft + destRect.width, destBottom = destTop + destRect.height;
    
    float positionsX[] { 0, centerLeft - texLeft, destRight - texRight + centerRight, destRight };
    float positionsY[] { 0, centerTop - texTop, destBottom - texBottom + centerBottom, destBottom };
    
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
