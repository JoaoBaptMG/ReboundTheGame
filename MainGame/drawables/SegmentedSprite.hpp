#pragma once

#include "Sprite.hpp"

class SegmentedSprite : public Sprite
{    
    sf::FloatRect centerRect, destRect;
    
protected:
    virtual void setupVertices() override;
    
public:
    SegmentedSprite(std::shared_ptr<sf::Texture> tex, sf::Vector2f anchor);
    SegmentedSprite(std::shared_ptr<sf::Texture> tex);
    SegmentedSprite();
    
    virtual ~SegmentedSprite() {}
    
    virtual void setTexture(std::shared_ptr<sf::Texture> tex) override
    { 
        centerRect = destRect = sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(getTextureSize()));
        Sprite::setTexture(tex);
    }
    
    virtual sf::FloatRect getBounds() const override;
    
    auto getCenterRect() const { return centerRect; }
    void setCenterRect(sf::FloatRect rect) { centerRect = rect; setupVertices(); }
    
    auto getDestinationRect() const { return destRect; }
    void setDestinationRect(sf::FloatRect rect) { destRect = rect; setupVertices(); }
};
