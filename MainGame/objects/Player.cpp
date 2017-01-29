#include "Player.hpp"

#include "utility/vector_math.hpp"
#include "input/PlayerController.hpp"
#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"

#include <functional>
#include <limits>
#include <cppmunk/Body.h>
#include <cppmunk/CircleShape.h>

Player::Player(const PlayerController& controller, GameScene& scene)
    : controller(controller)
{
    using namespace Chipmunk;
    
    sprite.setTexture(*scene.getResourceManager().load<sf::Texture>("player.png"));
    sprite.setOrigin(32, 32);

    auto body = std::make_shared<Body>(0.0, 0.0);
    playerShape = std::make_shared<CircleShape>(body, 32);
    playerShape->setDensity(1);
    playerShape->setElasticity(1);

    scene.getGameSpace().add(body);
    scene.getGameSpace().add(playerShape);

    body->setPosition({ 160, 160 });
    body->setMoment(std::numeric_limits<cpFloat>::infinity());
}

Player::~Player()
{
}

void Player::update(float dt)
{
    auto vec = controller.getMovementVector();
    auto body = playerShape->getBody();

    auto vel = body->getVelocity();
    auto dest = 320 * vec.x;

    if (std::abs(vel.x - dest) < 6.0)
    {
        vel.x = dest;
        body->setVelocity(vel);
    }

    auto base = cpVect{vel.x < dest ? 1.0 : vel.x > dest ? -1.0 : 0.0, 0.0};

    auto pos = body->getPosition();
    body->applyForceAtWorldPoint(base * 2400000.0, pos);

    if (controller.hasJumpAction())
        body->applyImpulseAtWorldPoint(cpVect{0, -600.0} * body->getMass(), pos);
}

void Player::render(Renderer& renderer)
{
    renderer.pushTransform();

    renderer.currentTransform.translate(getDisplayPosition());
    renderer.currentTransform.rotate(radiansToDegrees(playerShape->getBody()->getAngle()));

    renderer.pushDrawable(sprite, {}, 2);
    renderer.popTransform();
}

