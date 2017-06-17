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

    void setTexture(std::shared_ptr<sf::Texture> tex) { texture = tex; }
    void setTileData(const util::grid<uint8_t>& data) { tileData = data; }
    void setTileData(util::grid<uint8_t>&& data) { tileData = std::move(data); }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
