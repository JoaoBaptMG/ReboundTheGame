#pragma once

#include "objects/GameObject.hpp"
#include "drawables/Sprite.hpp"

#include <chipmunk/chipmunk.h>

#include <chronoUtils.hpp>
#include <cmath>

class GameScene;
class Renderer;

class Player;

class Bomb final : public GameObject
{
    Sprite sprite;
    cpVect position;
    FrameTime curTime, detonationTime;

public:
    Bomb(GameScene& scene, cpVect pos, FrameTime initialTime);
    virtual ~Bomb();

    virtual void update(FrameTime curTime) override;
    virtual void render(Renderer& renderer) override;
    virtual bool notifyScreenTransition(cpVect displacement) override;

    void setPosition(cpVect pos) { position = pos; }
    cpVect getPosition() const { return position; }

    auto getDisplayPosition() const
    {
        auto vec = getPosition();
        return sf::Vector2f((float)std::round(vec.x), (float)std::round(vec.y));
    }

    void detonate();

    static constexpr uint32_t InteractionType = 'bomb';
};
