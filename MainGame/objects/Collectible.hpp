#pragma once

#include "GameObject.hpp"
#include "scene/GameScene.hpp"

#include <memory>
#include <chipmunk/chipmunk.h>
#include <cppmunk.h>

class Player;

class Collectible : public GameObject
{
    static void setupCollisionHandlers(cp::Space* space);

protected:
    std::shared_ptr<cp::Body> collisionBody;

public:
    Collectible(GameScene& scene);
    virtual ~Collectible() {}

    void setupPhysics();

    virtual void onCollect(Player& player) = 0;

    virtual bool notifyScreenTransition(cpVect displacement);

    auto getPosition() const { return collisionBody->getPosition(); }
    auto getDisplayPosition() const
    {
        auto vec = getPosition();
        return sf::Vector2f((float)std::floor(vec.x), (float)std::floor(vec.y));
    }

    static constexpr cpCollisionType CollisionType = 'item';
};
