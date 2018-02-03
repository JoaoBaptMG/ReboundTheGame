#include "Enemy.hpp"

#include "objects/Player.hpp"

#include <cppmunk/Arbiter.h>
#include <cppmunk/Space.h>

using namespace cp;

void Enemy::setupCollisionHandlers(Space* space)
{
    if (!gameScene.getLevelPersistentData().existsDataOfType<Space*>("enemy.lastSetupSpace") ||
        gameScene.getLevelPersistentData().getData<Space*>("enemy.lastSetupSpace") != space)
    {
        space->addCollisionHandler(Player::CollisionType, CollisionType,
            [](Arbiter, Space&) { return true; },
            [](Arbiter arbiter, Space& space)
            {
                auto player = static_cast<Player*>(cpBodyGetUserData(arbiter.getBodyA()));
                auto enemy = static_cast<Enemy*>(cpBodyGetUserData(arbiter.getBodyB()));
                return enemy->onCollisionAttack(*player, space.findShape(arbiter.getShapeB()));
            }, [](Arbiter, Space&) {}, [](Arbiter, Space&) {});

        space->addCollisionHandler(Player::CollisionType, HitCollisionType,
            [](Arbiter, Space&) { return true; },
            [](Arbiter arbiter, Space& space)
            {
                auto player = static_cast<Player*>(cpBodyGetUserData(arbiter.getBodyA()));
                auto enemy = static_cast<Enemy*>(cpBodyGetUserData(arbiter.getBodyB()));
                return enemy->onCollisionHit(*player, space.findShape(arbiter.getShapeB()));
            }, [](Arbiter, Space&) {}, [](Arbiter, Space&) {});
        
        gameScene.getLevelPersistentData().setData("enemy.lastSetupSpace", space);
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
