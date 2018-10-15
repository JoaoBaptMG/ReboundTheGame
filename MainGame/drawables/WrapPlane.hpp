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
