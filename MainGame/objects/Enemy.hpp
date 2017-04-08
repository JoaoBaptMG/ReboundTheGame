#pragma once

#include <memory>

#include <chipmunk.h>
#include <cppmunk/Body.h>
#include <cppmunk/Space.h>

#include "objects/GameObject.hpp"
#include "scene/GameScene.hpp"

class Player;

class Enemy : public GameObject
{
    static void setupCollisionHandlers(Chipmunk::Space* space);
    
protected:
    std::shared_ptr<Chipmunk::Body> collisionBody;
    
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
        return sf::Vector2f(std::floor(vec.x), std::floor(vec.y));
    }

    static constexpr cpCollisionType CollisionType = 'enmy';
    static constexpr cpCollisionType HitCollisionType = 'ehit';
};
