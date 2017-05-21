#include "DestructibleCrate.hpp"

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
using namespace cp;

DestructibleCrate::DestructibleCrate(GameScene& gameScene, std::string texture, uint32_t type)
    : GameObject(gameScene), sprite(gameScene.getResourceManager().load<sf::Texture>(texture))
{
    setupPhysics();
    interactionHandler = [this,type] (uint32_t ty, void* ptr)
    {
        if (ty == type) remove();
    };
}

bool DestructibleCrate::configure(const DestructibleCrate::ConfigStruct& config)
{
    setPosition(cpVect{(cpFloat)config.position.x, (cpFloat)config.position.y});

    auto player = gameScene.getObjectByName<Player>("player");
    if (player)
    {
        auto dif = gameScene.wrapPosition(player->getPosition()) - getPosition();
        if (std::abs(dif.x) <= 48 && std::abs(dif.y) <= 48) return false;
    }

    return true;
}

void DestructibleCrate::setupPhysics()
{
    auto body = std::make_shared<Body>(Body::Kinematic);
    
    shape = std::make_shared<PolyShape>(body, std::vector<cpVect>
        { cpVect{-32, -32}, cpVect{+32, -32}, cpVect{+32, +32}, cpVect{-32, +32} }, 16);
    
    shape->setDensity(1);
    shape->setElasticity(0.6);
    shape->setCollisionType(Interactable);
    shape->setUserData(&interactionHandler);

    gameScene.getGameSpace().add(body);
    gameScene.getGameSpace().add(shape);

    body->setUserData((void*)this);
}

DestructibleCrate::~DestructibleCrate()
{
    if (shape)
    {
        auto body = shape->getBody();
        gameScene.getGameSpace().remove(shape);
        gameScene.getGameSpace().remove(body);
    }
}

void DestructibleCrate::update(std::chrono::steady_clock::time_point curTime)
{

}

void DestructibleCrate::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(getDisplayPosition());
    renderer.pushDrawable(sprite, {}, 25);
    renderer.popTransform();
}
