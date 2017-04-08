#include "BombCrate.hpp"

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "utility/vector_math.hpp"
#include "objects/Bomb.hpp"

#include <functional>
#include <limits>
#include <cppmunk/Body.h>
#include <cppmunk/PolyShape.h>
#include <cppmunk/Space.h>
#include <cppmunk/Arbiter.h>

using namespace props;
using namespace Chipmunk;

BombCrate::BombCrate(GameScene& gameScene) : GameObject(gameScene),
    sprite(gameScene.getResourceManager().load<sf::Texture>("bomb-crate.png"))
{
    setupPhysics();
    bombHandler = [this] (Bomb*) { remove(); };
}

bool BombCrate::configure(const BombCrate::ConfigStruct& config)
{
    setPosition(cpVect{(cpFloat)config.position.x, (cpFloat)config.position.y});

    return true;
}

void BombCrate::setupPhysics()
{
    auto body = std::make_shared<Body>(Body::Kinematic);
    
    shape = std::make_shared<PolyShape>(body, std::vector<cpVect>
        { cpVect{-32, -32}, cpVect{+32, -32}, cpVect{+32, +32}, cpVect{-32, +32} }, 16);
    
    shape->setDensity(1);
    shape->setElasticity(0.6);
    shape->setCollisionType(Bomb::Bombable);
    shape->setUserData(&bombHandler);

    gameScene.getGameSpace().add(body);
    gameScene.getGameSpace().add(shape);

    body->setUserData((void*)this);
}

BombCrate::~BombCrate()
{
    if (shape)
    {
        auto body = shape->getBody();
        gameScene.getGameSpace().remove(shape);
        gameScene.getGameSpace().remove(body);
    }
}

void BombCrate::update(std::chrono::steady_clock::time_point curTime)
{
}

void BombCrate::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(getDisplayPosition());
    renderer.pushDrawable(sprite, {}, 25);
    renderer.popTransform();
}
