#include "Collectible.hpp"

#include "objects/Player.hpp"

#include <cppmunk/Arbiter.h>
#include <cppmunk/Space.h>

using namespace Chipmunk;

void Collectible::setupCollisionHandlers(Space* space)
{
    static Space* lastSpaceSetup = nullptr;

    if (lastSpaceSetup != space)
    {
        space->addCollisionHandler(Player::CollisionType, CollisionType,
            [](Arbiter arbiter, Space&)
            {
                auto player = static_cast<Player*>(cpBodyGetUserData(arbiter.getBodyA()));
                auto collectible = static_cast<Collectible*>(cpBodyGetUserData(arbiter.getBodyB()));
                collectible->onCollect(*player);
                return false;
            }, [](Arbiter, Space&) { return false; }, [](Arbiter, Space&) {}, [](Arbiter, Space&) {});
        
        lastSpaceSetup = space;
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
