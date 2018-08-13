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


#include <chronoUtils.hpp>
#include <memory>
#include <rect.hpp>

enum class MeterSize { Small, Normal };

class GUIMeter final : public sf::Drawable
{
    std::shared_ptr<Texture> icon;
    
    MeterSize size;
    bool useCurrentAnimation;
    size_t height = 0, current = 0, target = 0;
    glm::u8vec4 fillColor, targetColor, backdropColor;

    sf::Vertex vertices[20];
    glm::vec2 position;

    void setRect(size_t ind, util::rect rect);
    void setQuadColor(size_t ind, glm::u8vec4 color);

public:
    GUIMeter(MeterSize size, bool useCurrentAnimation = true);
    virtual ~GUIMeter() {}

    size_t getHeight() const { return height; }
    void setHeight(size_t h) { height = h; updateVertices(); }

    size_t getCurrent() const { return current; }
    void setCurrent(size_t c) { current = c; }

    size_t getTarget() const { return target; }
    void setTarget(size_t t) { target = t; }
    
    float getFrameHeight() const;

    void setColors(glm::u8vec4 fill, glm::u8vec4 target, glm::u8vec4 backdrop)
    {
        fillColor = fill;
        targetColor = target;
        backdropColor = backdrop;
        updateVertices();
    }
    
    auto getFillColor() const { return fillColor; }
    void setFillColor(glm::u8vec4 color) { fillColor = color; updateVertices(); }
    
    auto getTargetColor() const { return targetColor; }
    void setTargetColor(glm::u8vec4 color) { targetColor = color; updateVertices(); }
    
    auto getBackdropColor() const { return backdropColor; }
    void setBackdropColor(glm::u8vec4 color) { backdropColor = color; updateVertices(); }

    auto getIcon() const { return icon; }
    void setIcon(const std::shared_ptr<Texture>& i) { icon = i; }

    auto getPosition() const { return position; }
    void setPosition(glm::vec2 pos) { position = pos; }
    void setPosition(float x, float y) { position = glm::vec2(x, y); }

    void update(FrameTime);
    void updateVertices();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
