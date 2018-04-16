//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//


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
