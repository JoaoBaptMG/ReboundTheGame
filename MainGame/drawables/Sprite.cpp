#include "Sprite.hpp"

static const sf::Vertex quad[4] =
{
    sf::Vertex(sf::Vector2f(0, 0), sf::Vector2f(0, 0)),
    sf::Vertex(sf::Vector2f(0, 1), sf::Vector2f(0, 1)),
    sf::Vertex(sf::Vector2f(1, 1), sf::Vector2f(1, 1)),
    sf::Vertex(sf::Vector2f(1, 0), sf::Vector2f(1, 0)),
};

Sprite::Sprite(std::shared_ptr<sf::Texture> texture, sf::Vector2f anchor) : texture(texture), anchorPoint(anchor)
{
}

Sprite::Sprite(std::shared_ptr<sf::Texture> texture) : texture(texture)
{
    auto size = texture->getSize();
    anchorPoint = sf::Vector2f((float)size.x/2.0f, (float)size.y/2.0f);
}

void Sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    auto size = texture->getSize();

    states.blendMode = sf::BlendAlpha;
    states.transform.translate(anchorPoint);
    states.transform.scale((float)size.x, (float)size.y);
    states.texture = texture.get();

    target.draw(quad, 4, sf::PrimitiveType::TriangleFan, states);
}
