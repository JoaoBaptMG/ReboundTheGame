#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

class WrapPlane final : public sf::Drawable
{
    std::shared_ptr<sf::Texture> texture;
    sf::FloatRect drawArea;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
    WrapPlane(sf::FloatRect area, std::shared_ptr<sf::Texture> tex = nullptr) : texture(tex), drawArea(area) {}

    auto getTexture() { return texture; }
    void setTexture(std::shared_ptr<sf::Texture> tex)
    {
        texture = tex;
        texture->setRepeated(true);
    }

    auto getDrawArea() { return drawArea; }
    void setDrawArea(sf::FloatRect area) { drawArea = area; }
};
