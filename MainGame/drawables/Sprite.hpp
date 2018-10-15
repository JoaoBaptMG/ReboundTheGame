//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

class Sprite : public sf::Drawable
{
    static sf::Shader& getSpriteShader();
    
    std::shared_ptr<sf::Texture> texture;
    sf::Vector2f anchorPoint;

    sf::Color flashColor, blendColor;
    sf::FloatRect texRect;
    float opacity, grayscaleFactor;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    
protected:
    sf::VertexArray vertices;
    virtual void setupVertices();

public:
    Sprite(std::shared_ptr<sf::Texture> tex, sf::Vector2f anchor);
    Sprite(std::shared_ptr<sf::Texture> tex);
    Sprite();
    
    virtual ~Sprite() {}

    sf::Vector2f getAnchorPoint() const { return anchorPoint; }
    void setAnchorPoint(sf::Vector2f ap) { anchorPoint = ap; }
    void centerAnchorPoint()
    { 
        setAnchorPoint(sf::Vector2f(texRect.left + texRect.width/2, texRect.top + texRect.height/2));
    }

    sf::Vector2u getTextureSize() const
    {
        return texture ? texture->getSize() : sf::Vector2u(0, 0);
    }
    
    virtual sf::FloatRect getBounds() const;

    auto getFlashColor() const { return flashColor; }
    void setFlashColor(sf::Color color) { flashColor = color; }

    auto getBlendColor() const { return blendColor; }
    void setBlendColor(sf::Color color) { blendColor = color; }

    auto getOpacity() const { return opacity; }
    void setOpacity(float op) { opacity = op > 1 ? 1 : op < 0 ? 0 : op; }
    
    auto getGrayscaleFactor() const { return grayscaleFactor; }
    void setGrayscaleFactor(float gf) { grayscaleFactor = gf > 1 ? 1 : gf < 0 ? 0 : gf; }

    auto getTexture() const { return texture; }
    virtual void setTexture(std::shared_ptr<sf::Texture> tex)
    { 
        texture = tex;
        texRect = sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(getTextureSize()));
        setupVertices();
    }
    
    auto getTextureRect() const { return texRect; }
    void setTextureRect(sf::FloatRect rect) { texRect = rect; setupVertices(); }
};
