#include "Floater.hpp"

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include <chronoUtils.hpp>

#include "objects/GameObjectFactory.hpp"

#include <cppmunk/CircleShape.h>
#include <cppmunk/PolyShape.h>

using namespace enemies;

constexpr float Period = 6;
constexpr float Amplitude = 18;

Floater::Floater(GameScene& gameScene) : Enemy(gameScene),
    sprite(gameScene.getResourceManager().load<sf::Texture>("floater.png"))
{
    setupPhysics();
}

bool Floater::configure(const Floater::ConfigStruct& config)
{
    originalPos = cpVect{ (float)config.position.x, (float)config.position.y };
    collisionBody->setPosition(originalPos);
    
    return true;
}

void Floater::setupPhysics()
{
    using namespace cp;
    
    collisionBody = std::make_shared<Body>(Body::Kinematic);
    
    collisionShapes[0] = std::make_shared<CircleShape>(collisionBody, 23.0, cpVect{ 0.0, -25.0 });
    collisionShapes[1] = std::make_shared<PolyShape>(collisionBody, std::vector<cpVect>
        { { -18, -11 }, { -15, -2 }, { 15, -2 }, { 18, -11 } });
    collisionShapes[2] = std::make_shared<PolyShape>(collisionBody, std::vector<cpVect>
        { { -15, -2 }, { -15, 6 }, { 15, 6 }, { 15, -2 } });
    collisionShapes[3] = std::make_shared<PolyShape>(collisionBody, std::vector<cpVect>
        { { -15, 6 }, { -24, 15 }, { -24, 21 }, { 0, 48 },
          { 24, 21 }, { 24, 15 }, { 15, 6 } });
          
    gameScene.getGameSpace().add(collisionBody);
    for (const auto& shape : collisionShapes)
    {
        shape->setCollisionType(Enemy::CollisionType);
        
        shape->setElasticity(1.0);
        shape->setFriction(0.0);
        gameScene.getGameSpace().add(shape);
    }

    collisionShapes[0]->setCollisionType(Enemy::HitCollisionType);

    Enemy::setupPhysics();
}

Floater::~Floater()
{
    for (const auto& shape : collisionShapes)
        gameScene.getGameSpace().remove(shape);
    gameScene.getGameSpace().remove(collisionBody);
}

bool Floater::onCollisionAttack(Player& player, std::shared_ptr<cp::Shape> shape)
{
    player.damage(25);
    return true;
}

bool Floater::onCollisionHit(Player& player, std::shared_ptr<cp::Shape> shape)
{
    remove();
    return true;
}

void Floater::update(FrameTime curTime)
{
    if (initialTime == decltype(initialTime)())
        initialTime = curTime;

    float duration = toSeconds<float>(curTime - initialTime);
    float yDisplacement = -Amplitude * sinf(6.28318530718 * duration / Period);

    cpVect pos = originalPos + cpVect{ 0, yDisplacement };
    auto dx = pos - collisionBody->getPosition();
    collisionBody->setVelocity(dx * (1.0 / toSeconds<cpFloat>(UpdatePeriod)));
}

bool Floater::notifyScreenTransition(cpVect displacement)
{
    originalPos = originalPos + displacement;
    return Enemy::notifyScreenTransition(displacement);
}

void Floater::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(getDisplayPosition());
    renderer.pushDrawable(sprite, {}, 25);
    renderer.popTransform();
}

REGISTER_GAME_OBJECT(enemies::Floater);
