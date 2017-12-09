#include "Enemy.hpp"

#include "objects/Player.hpp"

#include <cppmunk/Arbiter.h>
#include <cppmunk/Space.h>

using namespace cp;

void Enemy::setupCollisionHandlers(Space* space)
{
    static Space* lastSpaceSetup = nullptr;

    if (lastSpaceSetup != space)
    {
        space->addCollisionHandler(Player::CollisionType, CollisionType,
            [](Arbiter, Space&) { return true; },
            [](Arbiter arbiter, Space&)
            {
                auto player = static_cast<Player*>(cpBodyGetUserData(arbiter.getBodyA()));
                auto enemy = static_cast<Enemy*>(cpBodyGetUserData(arbiter.getBodyB()));
                return enemy->onCollisionAttack(*player);
            }, [](Arbiter, Space&) {}, [](Arbiter, Space&) {});

        space->addCollisionHandler(Player::CollisionType, HitCollisionType,
            [](Arbiter, Space&) { return true; },
            [](Arbiter arbiter, Space&)
            {
                auto player = static_cast<Player*>(cpBodyGetUserData(arbiter.getBodyA()));
                auto enemy = static_cast<Enemy*>(cpBodyGetUserData(arbiter.getBodyB()));
                return enemy->onCollisionHit(*player);
            }, [](Arbiter, Space&) {}, [](Arbiter, Space&) {});
        
        lastSpaceSetup = space;
    }
}

Enemy::Enemy(GameScene& gameScene) : GameObject(gameScene)
{
    setupCollisionHandlers(&gameScene.getGameSpace());
}

void Enemy::setupPhysics()
{
    if (collisionBody) collisionBody->setUserData((void*)this);
}

bool Enemy::notifyScreenTransition(cpVect displacement)
{
    collisionBody->setPosition(collisionBody->getPosition() + displacement);
    return true;
}
