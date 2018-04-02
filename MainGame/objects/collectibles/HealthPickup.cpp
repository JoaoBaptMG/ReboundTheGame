#include "HealthPickup.hpp"

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include <chronoUtils.hpp>

#include "objects/GameObjectFactory.hpp"

#include <cppmunk/CircleShape.h>

using namespace collectibles;

HealthPickup::HealthPickup(GameScene& scene, size_t amount) : Collectible(scene), healthAmount(amount),
    sprite(gameScene.getResourceManager().load<sf::Texture>("health-pickup.png"))
{
    setupPhysics();
}

HealthPickup::HealthPickup(GameScene& scene) : HealthPickup(scene, 0) {}

bool HealthPickup::configure(const HealthPickup::ConfigStruct& config)
{
    auto pos = cpVect{ (float)config.position.x, (float)config.position.y };
    collisionBody->setPosition(pos);

    healthAmount = config.healthAmount;
    
    return true;
}

void HealthPickup::setupPhysics()
{
    using namespace cp;

    collisionBody = std::make_shared<Body>(Body::Kinematic);
    collisionShape = std::make_shared<CircleShape>(collisionBody, 16, cpVect{ 0.0, 0.0 });

    collisionShape->setCollisionType(Collectible::CollisionType);
    collisionShape->setSensor(true);

    gameScene.getGameSpace().add(collisionBody);
    gameScene.getGameSpace().add(collisionShape);
    
    Collectible::setupPhysics();
}

HealthPickup::~HealthPickup()
{
    gameScene.getGameSpace().remove(collisionShape);
    gameScene.getGameSpace().remove(collisionBody);
}

void HealthPickup::onCollect(Player& player)
{
    player.heal(healthAmount);
    remove();
}

void HealthPickup::update(FrameTime curTime)
{
}

void HealthPickup::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(getDisplayPosition());
    renderer.pushDrawable(sprite, {}, 25);
    renderer.popTransform();
}

REGISTER_GAME_OBJECT(collectibles::HealthPickup);
