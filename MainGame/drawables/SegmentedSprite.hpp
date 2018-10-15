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
