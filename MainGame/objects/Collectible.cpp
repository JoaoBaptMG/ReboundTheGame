#include "Collectible.hpp"

#include "objects/Player.hpp"

#include <cppmunk.h>

using namespace cp;

void Collectible::setupCollisionHandlers(Space* space)
{
    if (!gameScene.getLevelPersistentData().existsDataOfType<Space*>("collectible.lastSetupSpace") ||
        gameScene.getLevelPersistentData().getData<Space*>("collectible.lastSetupSpace") != space)
    {
        space->addCollisionHandler(Player::CollisionType, CollisionType,
            [](Arbiter arbiter, Space&)
            {
                auto player = static_cast<Player*>(cpBodyGetUserData(arbiter.getBodyA()));
                auto collectible = static_cast<Collectible*>(cpBodyGetUserData(arbiter.getBodyB()));
                collectible->onCollect(*player);
                return false;
            }, [](Arbiter, Space&) { return false; }, [](Arbiter, Space&) {}, [](Arbiter, Space&) {});
        
        gameScene.getLevelPersistentData().setData("collectible.lastSetupSpace", space);
    }
}

Collectible::Collectible(GameScene& scene) : GameObject(scene)
{
    isPersistent = false;
    setupCollisionHandlers(&gameScene.getGameSpace());
}

void Collectible::setupPhysics()
{
    if (collisionBody) collisionBody->setUserData((void*)this);
}

bool Collectible::notifyScreenTransition(cpVect displacement)
{
    collisionBody->setPosition(collisionBody->getPosition() + displacement);
    return true;
}
