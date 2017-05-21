#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

class WrapPlane final : public sf::Drawable
{
    std::shared_ptr<sf::Texture> texture;
    sf::FloatRect drawArea;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
    WrapPlane(sf::FloatRect area, std::shared_ptr<sf::Texture> tex = nullptr) : drawArea(area) { setTexture(tex); }

    auto getTexture() { return texture; }
    void setTexture(std::shared_ptr<sf::Texture> tex)
    {
        texture = tex;
        if (texture) texture->setRepeated(true);
    }

    auto getDrawArea() { return drawArea; }
    void setDrawArea(sf::FloatRect area) { drawArea = area; }
};
