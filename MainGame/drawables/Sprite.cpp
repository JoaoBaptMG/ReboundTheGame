#include "Sprite.hpp"

Sprite::Sprite(std::shared_ptr<sf::Texture> texture, sf::Vector2f anchor) : texture(texture), anchorPoint(anchor)
{
    setupVertices();
}

Sprite::Sprite(std::shared_ptr<sf::Texture> texture) : Sprite(texture, sf::Vector2f(texture->getSize())/2.0f)
{
}

sf::FloatRect Sprite::getTextureBounds() const
{
    return sf::FloatRect(-anchorPoint, (sf::Vector2f)getTextureSize());
}

void Sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!texture) return;
    
    states.blendMode = sf::BlendAlpha;
    states.transform.translate(-anchorPoint);
    states.texture = texture.get();

    target.draw(quad, 4, sf::TriangleFan, states);
}

void Sprite::setupVertices()
{
    for (auto& vtx : quad) vtx.color = sf::Color::White;

    auto size = getTextureSize();

    quad[0].position = quad[0].texCoords = sf::Vector2f(0, 0);
    quad[1].position = quad[1].texCoords = sf::Vector2f(size.x, 0);
    quad[2].position = quad[2].texCoords = sf::Vector2f(size.x, size.y);
    quad[3].position = quad[3].texCoords = sf::Vector2f(0, size.y);
}
