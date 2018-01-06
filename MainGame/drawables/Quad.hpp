#pragma once

#include <SFML/Graphics.hpp>

class Quad : public sf::Drawable
{
    sf::Color color;
    sf::FloatRect rect;
    
    sf::VertexArray vertices;
    void setupVertices();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
    Quad(sf::FloatRect rect, sf::Color color = sf::Color());
    virtual ~Quad() {}
    
    auto getRect() const { return rect; }
    void setRect(sf::FloatRect r) { rect = r; setupVertices(); }
    
    auto getColor() const { return color; }
    void setColor(sf::Color c) { color = c; setupVertices(); }
    
    auto getAlpha() const { return color.a; }
    void setAlpha(uint8_t a) { color.a = a; setupVertices(); }
};
