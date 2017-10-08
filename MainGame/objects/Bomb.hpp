#pragma once

#include "objects/GameObject.hpp"
#include "drawables/Sprite.hpp"

#include <chipmunk/chipmunk.h>

#include <chrono>

class GameScene;
class Renderer;

class Player;

class Bomb final : public GameObject
{
    Sprite sprite;
    cpVect position;
    std::chrono::steady_clock::time_point detonationTime;
    Player* player;

public:
    Bomb(GameScene& scene, cpVect pos, Player* player, std::chrono::steady_clock::time_point initialTime);
    virtual ~Bomb() {}

    virtual void update(std::chrono::steady_clock::time_point curTime) override;
    virtual void render(Renderer& renderer) override;

    void setPosition(cpVect pos) { position = pos; }
    cpVect getPosition() const { return position; }

    auto getDisplayPosition() const
    {
        auto vec = getPosition();
        return sf::Vector2f((float)std::floor(vec.x), (float)std::floor(vec.y));
    }

    void detonate();

    static constexpr uint32_t InteractionType = 'bomb';
};
