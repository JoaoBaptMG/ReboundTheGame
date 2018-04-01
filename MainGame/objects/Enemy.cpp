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
        auto attackHandler = [](Arbiter arbiter, Space& space)
        {
            auto player = static_cast<Player*>(cpBodyGetUserData(arbiter.getBodyA()));
            auto enemy = static_cast<Enemy*>(cpBodyGetUserData(arbiter.getBodyB()));
            return enemy->onCollisionAttack(*player, space.findShape(arbiter.getShapeB()));
        };
        
        auto hitHandler = [](Arbiter arbiter, Space& space)
        {
            auto player = static_cast<Player*>(cpBodyGetUserData(arbiter.getBodyA()));
            auto enemy = static_cast<Enemy*>(cpBodyGetUserData(arbiter.getBodyB()));
            return enemy->onCollisionHit(*player, space.findShape(arbiter.getShapeB()));
        };
        
        auto groundStandHandler = [] (Arbiter arbiter, Space&)
        {
            auto normal = arbiter.getNormal();
            if (normal.y * 0.75 >= fabs(normal.x))
                arbiter.setRestitution(0);
            return true;
        };
        
        space->addCollisionHandler(Player::CollisionType, CollisionType,
            [](Arbiter, Space&) { return true; }, attackHandler, [](Arbiter, Space&) {}, [](Arbiter, Space&) {});
        space->addCollisionHandler(Player::CollisionType, GroundStandCollisionType,
            [](Arbiter, Space&) { return true; }, attackHandler, [](Arbiter, Space&) {}, [](Arbiter, Space&) {});

        space->addCollisionHandler(Player::CollisionType, HitCollisionType,
            [](Arbiter, Space&) { return true; }, hitHandler, [](Arbiter, Space&) {}, [](Arbiter, Space&) {});
        space->addCollisionHandler(Player::CollisionType, GroundStandHitCollisionType,
            [](Arbiter, Space&) { return true; }, hitHandler, [](Arbiter, Space&) {}, [](Arbiter, Space&) {});
        
        space->addWildcardCollisionHandler(GroundStandCollisionType, [] (Arbiter, Space&) { return true; },
            groundStandHandler, [] (Arbiter, Space&) {}, [] (Arbiter, Space&) {});
        space->addWildcardCollisionHandler(GroundStandHitCollisionType, [] (Arbiter, Space&) { return true; },
            groundStandHandler, [] (Arbiter, Space&) {}, [] (Arbiter, Space&) {});
        
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
