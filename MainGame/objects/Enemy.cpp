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
