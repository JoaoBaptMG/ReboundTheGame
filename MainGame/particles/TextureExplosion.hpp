#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <chrono>
#include <memory>

#include "objects/GameObject.hpp"

class GameScene;
class Renderer;

class TextureExplosion final : public GameObject
{
public:
    using TimePoint = std::chrono::steady_clock::time_point;
    using Duration = TimePoint::duration;

    struct TexPiece
    {
        sf::Vector2f position, velocity;
    };

private:
    std::shared_ptr<sf::Texture> texture;
    std::vector<TexPiece> texturePieces;
    sf::VertexArray vertices;
    sf::Vector2f globalPosition, globalAcceleration;
    size_t width, height, drawingDepth;
    TimePoint curTime, lastTime, initialTime;
    Duration duration;
    
public:
    const static struct DensityTag {} Density;
    const static struct SizeTag {} Size;

    TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, Duration duration,
        sf::FloatRect velocityRect, sf::Vector2f acceleration, SizeTag, size_t width, size_t height,
        size_t depth = 12);
    TextureExplosion(GameScene& scene, std::shared_ptr<sf::Texture> tex, Duration duration,
        sf::FloatRect velocityRect, sf::Vector2f acceleration, DensityTag, size_t pieceSizeX, size_t pieceSizeY,
        size_t depth = 12);

    auto getPosition() { return globalPosition; }
    void setPosition(sf::Vector2f pos) { globalPosition = pos; }

    virtual void update(std::chrono::steady_clock::time_point curTime) override;
    virtual void render(Renderer& renderer) override;
};
