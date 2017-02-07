#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

class Sprite final : public sf::Drawable
{
    std::shared_ptr<sf::Texture> texture;
    sf::Vector2f anchorPoint;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
    Sprite(std::shared_ptr<sf::Texture> tex, sf::Vector2f anchor);
    Sprite(std::shared_ptr<sf::Texture> tex);

    virtual ~Sprite() {}
};
