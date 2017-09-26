#include "DestructibleCrate.hpp"

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include <vector_math.hpp>
#include "objects/Bomb.hpp"
#include "particles/TextureExplosion.hpp"

#include <functional>
#include <limits>
#include <cppmunk/Body.h>
#include <cppmunk/PolyShape.h>
#include <cppmunk/Space.h>
#include <cppmunk/Arbiter.h>

using namespace props;
using namespace cp;
using namespace std::literals::chrono_literals;

const sf::FloatRect velocityRect(-64, -384, 128, 128);
constexpr auto ExplosionDuration = 2s;

DestructibleCrate::DestructibleCrate(GameScene& gameScene, std::string texture, uint32_t type)
    : GameObject(gameScene), sprite(gameScene.getResourceManager().load<sf::Texture>(texture))
{
    setupPhysics();
    interactionHandler = [this,type] (uint32_t ty, void* ptr)
    {
        if (ty == type)
        {
            explode();
            remove();
        }
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

void DestructibleCrate::explode()
{
    auto grav = gameScene.getGameSpace().getGravity();
    auto displayGravity = sf::Vector2f(grav.x, grav.y);
    
    auto explosion = std::make_unique<TextureExplosion>(gameScene, sprite.getTexture(), ExplosionDuration,
        velocityRect, displayGravity, TextureExplosion::Density, 8, 8, 25);
    explosion->setPosition(getDisplayPosition());
    gameScene.addObject(std::move(explosion));
}

void DestructibleCrate::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(getDisplayPosition());
    renderer.pushDrawable(sprite, {}, 25);
    renderer.popTransform();
}
