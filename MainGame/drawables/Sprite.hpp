#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

class Sprite final : public sf::Drawable
{
    static sf::Shader& getSpriteShader();
    
    std::shared_ptr<sf::Texture> texture;
    sf::Vector2f anchorPoint;

    sf::Vertex quad[4];
    sf::Color flashColor, blendColor;
    float opacity;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void setupVertices();

public:
    Sprite(std::shared_ptr<sf::Texture> tex, sf::Vector2f anchor);
    Sprite(std::shared_ptr<sf::Texture> tex = nullptr);

    sf::Vector2f getAnchorPoint() const { return anchorPoint; }
    void setAnchorPoint(sf::Vector2f ap) { anchorPoint = ap; }

    auto getTextureSize() const { return texture->getSize(); }
    sf::FloatRect getTextureBounds() const;

    auto getFlashColor() const { return flashColor; }
    void setFlashColor(sf::Color color) { flashColor = color; }

    auto getBlendColor() const { return blendColor; }
    void setBlendColor(sf::Color color) { blendColor = color; }

    auto getOpacity() const { return opacity; }
    void setOpacity(float op) { opacity = op; }

    auto getTexture() const { return texture; }
    void setTexture(std::shared_ptr<sf::Texture> tex) { texture = tex; setupVertices(); }

    virtual ~Sprite() {}
};
