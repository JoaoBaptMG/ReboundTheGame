#pragma once

#include "GameObject.hpp"
#include "scene/GameScene.hpp"

#include <memory>
#include <chipmunk.h>
#include <cppmunk/Body.h>
#include <cppmunk/Space.h>

class Player;

class Collectible : public GameObject
{
    static void setupCollisionHandlers(Chipmunk::Space* space);

protected:
    std::shared_ptr<Chipmunk::Body> collisionBody;

public:
    Collectible(GameScene& scene);
    virtual ~Collectible() {}

    void setupPhysics();

    virtual void onCollect(Player& player) = 0;

    auto getPosition() const { return collisionBody->getPosition(); }
    auto getDisplayPosition() const
    {
        auto vec = getPosition();
        return sf::Vector2f(std::floor(vec.x), std::floor(vec.y));
    }

    static constexpr cpCollisionType CollisionType = 'item';
};
