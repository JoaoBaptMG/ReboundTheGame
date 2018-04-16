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
#include <SFML/Graphics.hpp>
#include <non_copyable_movable.hpp>
#include <grid.hpp>
#include "defaults.hpp"

class Tilemap final : public sf::Drawable
{
    std::shared_ptr<sf::Texture> texture;

    mutable std::unique_ptr<sf::Vertex[]> vertices;
    mutable size_t vertexSize;
    mutable sf::Vector2i lastPoint;

    sf::FloatRect drawingFrame;
    size_t tileSize;

    util::grid<uint8_t> tileData;

    // "const", because it modifies mutable parameters
    void mutableUpdateVertexMap(sf::Transform transform) const;

public:
    explicit Tilemap(sf::FloatRect drawingFrame, size_t tileSize = DefaultTileSize)
    : texture(nullptr), vertices(nullptr), vertexSize(0), drawingFrame(drawingFrame),
      tileSize(tileSize), tileData(), lastPoint(0,0) {}
    explicit Tilemap(size_t tileSize = DefaultTileSize) : Tilemap(sf::FloatRect{}, tileSize) {}
      
    virtual ~Tilemap() {}

    void setDrawingFrame(sf::FloatRect drawingFrame) { this->drawingFrame = drawingFrame; }

    void setTexture(std::shared_ptr<sf::Texture> tex) { texture = tex;  }
    void setTileData(const util::grid<uint8_t>& data) { tileData = data; vertexSize = 0; }
    void setTileData(util::grid<uint8_t>&& data) { tileData = std::move(data); vertexSize = 0; }
    
    auto getTexture() { return texture; }
    const auto& getTileData() { return tileData; }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
