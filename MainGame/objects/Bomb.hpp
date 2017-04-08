#pragma once

#include "objects/GameObject.hpp"
#include "drawables/Sprite.hpp"

#include <chipmunk.h>

#include <chrono>

class GameScene;
class Renderer;

class Bomb final : public GameObject
{
    Sprite sprite;
    cpVect position;
    std::chrono::steady_clock::time_point detonationTime;

public:
    Bomb(GameScene& scene, cpVect pos, std::chrono::steady_clock::time_point initialTime);
    ~Bomb() {}

    virtual void update(std::chrono::steady_clock::time_point curTime) override;
    virtual void render(Renderer& renderer) override;

    void setPosition(cpVect pos) { position = pos; }
    cpVect getPosition() const { return position; }

    auto getDisplayPosition() const
    {
        auto vec = getPosition();
        return sf::Vector2f(std::floor(vec.x), std::floor(vec.y));
    }

    void detonate();

    static constexpr cpCollisionType Bombable = 'bomb';
};
