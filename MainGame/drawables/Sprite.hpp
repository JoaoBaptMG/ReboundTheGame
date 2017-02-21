#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

class Sprite final : public sf::Drawable
{
    std::shared_ptr<sf::Texture> texture;
    sf::Vector2f anchorPoint;

    sf::Vertex quad[4];

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void setupVertices();

public:
    Sprite(std::shared_ptr<sf::Texture> tex, sf::Vector2f anchor);
    Sprite(std::shared_ptr<sf::Texture> tex);

    sf::Vector2f getAnchorPoint() const { return anchorPoint; }
    void setAnchorPoint(sf::Vector2f ap) { anchorPoint = ap; }

    auto getTextureSize() const { return texture->getSize(); }
    sf::FloatRect getTextureBounds() const;

    auto getTexture() const { return texture; }
    void setTexture(std::shared_ptr<sf::Texture> tex) { texture = tex; }

    virtual ~Sprite() {}
};
