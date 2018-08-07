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


#include "Rotator.hpp"

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include <chronoUtils.hpp>
#include <vector_math.hpp>

#include "objects/GameObjectFactory.hpp"

#include <cppmunk/CircleShape.h>
#include <cppmunk/PolyShape.h>

using namespace enemies;

constexpr float Speed = 160;

Rotator::Rotator(GameScene& gameScene) : Enemy(gameScene),
    sprite(gameScene.getResourceManager().load<sf::Texture>("rotator.png"))
{
    setupPhysics();
    collisionBody->setAngularVelocity(4.4);
}

bool Rotator::configure(const Rotator::ConfigStruct &config)
{
    auto pos = cpVect{ (float)config.position.x, (float)config.position.y };
    collisionBody->setPosition(pos);

    wanderingArea = cpBBNewForExtents(pos, (float)config.width/2, (float)config.height/2);

    auto spd = cpVect{ 0, 0 };
    if (config.width != 0) spd.x = Speed;
    if (config.height != 0) spd.y = Speed;

    collisionBody->setVelocity(spd);
    
    return true;
}

void Rotator::setupPhysics()
{
    using namespace cp;
    
    collisionBody = std::make_shared<Body>(Body::Kinematic);
    
    collisionShapes[0] = std::make_shared<CircleShape>(collisionBody, 23.0, cpVect{ 0.0, 0.0 });
    collisionShapes[1] = std::make_shared<PolyShape>(collisionBody, std::vector<cpVect>
        { { 16, 17 }, { 57.5, 0 }, { 16, -17 } });
    collisionShapes[2] = std::make_shared<PolyShape>(collisionBody, std::vector<cpVect>
        { { -16, -17 }, { -57.5, 0 }, { -16, 17 } });
          
    gameScene.getGameSpace().add(collisionBody);
    for (const auto& shape : collisionShapes)
    {
        shape->setCollisionType(Enemy::CollisionType);
        
        shape->setElasticity(0.3);
        shape->setFriction(0.0);
        gameScene.getGameSpace().add(shape);
    }

    collisionShapes[0]->setCollisionType(Enemy::HitCollisionType);

    Enemy::setupPhysics();
}

Rotator::~Rotator()
{
    for (const auto& shape : collisionShapes)
        gameScene.getGameSpace().remove(shape);
    gameScene.getGameSpace().remove(collisionBody);
}


bool Rotator::onCollisionAttack(Player& player, std::shared_ptr<cp::Shape> shape)
{
    player.damage(10);
    return true;
}

bool Rotator::onCollisionHit(Player& player, std::shared_ptr<cp::Shape> shape)
{
    remove();
    return true;
}

void Rotator::update(FrameTime curTime)
{
    auto pos = collisionBody->getPosition();
    auto spd = collisionBody->getVelocity();

    if (pos.x < wanderingArea.l || pos.x > wanderingArea.r)
        spd.x = -spd.x;
    if (pos.y < wanderingArea.b || pos.y > wanderingArea.t)
        spd.y = -spd.y;

    collisionBody->setVelocity(spd);
}

bool Rotator::notifyScreenTransition(cpVect displacement)
{
    wanderingArea.l += displacement.x;
    wanderingArea.r += displacement.x;
    wanderingArea.b += displacement.y;
    wanderingArea.t += displacement.y;
    return Enemy::notifyScreenTransition(displacement);
}

void Rotator::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform *= util::translate(getDisplayPosition());
    renderer.currentTransform *= util::rotate(collisionBody->getAngle());
    renderer.pushDrawable(sprite, {}, 25);
    renderer.popTransform();
}

REGISTER_GAME_OBJECT(enemies::Rotator);
