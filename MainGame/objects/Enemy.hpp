#pragma once

#include <memory>

#include <chipmunk/chipmunk.h>
#include <cppmunk/Body.h>
#include <cppmunk/Space.h>

#include "objects/GameObject.hpp"
#include "scene/GameScene.hpp"

class Player;

class Enemy : public GameObject
{
    static void setupCollisionHandlers(cp::Space* space);
    
protected:
    std::shared_ptr<cp::Body> collisionBody;
    
public:
    Enemy(GameScene& scene);
    virtual ~Enemy() {}

    void setupPhysics();

    virtual bool onCollisionAttack(Player& player) = 0;
    virtual bool onCollisionHit(Player& player) = 0;

    auto getPosition() const { return collisionBody->getPosition(); }
    auto getDisplayPosition() const
    {
        auto vec = getPosition();
        return sf::Vector2f((float)std::round(vec.x), (float)std::round(vec.y));
    }

    virtual bool notifyScreenTransition(cpVect displacement);

    static constexpr cpCollisionType CollisionType = 'enmy';
    static constexpr cpCollisionType HitCollisionType = 'ehit';
};
