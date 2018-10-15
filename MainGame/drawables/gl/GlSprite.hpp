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


#include <memory>
#include <glm/glm.hpp>
#include <rect.hpp>

class Sprite : public sf::Drawable
{
    static sf::Shader& getSpriteShader();
    
    std::shared_ptr<Texture> texture;
    glm::vec2 anchorPoint;

    glm::u8vec4 flashColor, blendColor;
    util::rect texRect;
    float opacity, grayscaleFactor;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    
protected:
    sf::VertexArray vertices;
    virtual void setupVertices();

public:
    Sprite(std::shared_ptr<Texture> tex, glm::vec2 anchor);
    Sprite(std::shared_ptr<Texture> tex);
    Sprite();
    
    virtual ~Sprite() {}

    glm::vec2 getAnchorPoint() const { return anchorPoint; }
    void setAnchorPoint(glm::vec2 ap) { anchorPoint = ap; }
    void centerAnchorPoint()
    { 
        setAnchorPoint(glm::vec2(texRect.x + texRect.width/2, texRect.y + texRect.height/2));
    }

    glm::uvec2 getTextureSize() const
    {
		return glm::uvec2(texture ? texture->getSize().x : 0, texture ? texture->getSize().y : 0);
    }
    
    virtual util::rect getBounds() const;

    auto getFlashColor() const { return flashColor; }
    void setFlashColor(glm::u8vec4 color) { flashColor = color; }

    auto getBlendColor() const { return blendColor; }
    void setBlendColor(glm::u8vec4 color) { blendColor = color; }

    auto getOpacity() const { return opacity; }
    void setOpacity(float op) { opacity = op > 1 ? 1 : op < 0 ? 0 : op; }
    
    auto getGrayscaleFactor() const { return grayscaleFactor; }
    void setGrayscaleFactor(float gf) { grayscaleFactor = gf > 1 ? 1 : gf < 0 ? 0 : gf; }

    auto getTexture() const { return texture; }
    virtual void setTexture(std::shared_ptr<Texture> tex)
    { 
        texture = tex;
        texRect = util::rect(glm::vec2(0, 0), glm::vec2(getTextureSize().x, getTextureSize().y));
        setupVertices();
    }
    
    auto getTextureRect() const { return texRect; }
    void setTextureRect(util::rect rect) { texRect = rect; setupVertices(); }
};
