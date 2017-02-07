#include "Player.hpp"

#include "utility/vector_math.hpp"
#include "input/PlayerController.hpp"
#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "defaults.hpp"
#include "utility/assert.hpp"

#include <functional>
#include <limits>
#include <cppmunk/Body.h>
#include <cppmunk/CircleShape.h>
#include <cppmunk/Space.h>
#include <cppmunk/Arbiter.h>

using namespace std::literals::chrono_literals;

Player::Player(GameScene& scene)
    : abilityLevel(0), GameObject(scene)
{
    sprite.setTexture(*scene.getResourceManager().load<sf::Texture>("player.png"));
    sprite.setOrigin(32, 32);

    setupPhysics();

    setName("player");
}

bool Player::configure(const Player::ConfigStruct& config)
{
    playerShape->getBody()->setPosition(cpVect{config.position.x, config.position.y});
    return gameScene.getObjectByName("player") == nullptr;
}

void Player::setupPhysics()
{
    using namespace Chipmunk;
    
    auto body = std::make_shared<Body>(0.0, 0.0);
    playerShape = std::make_shared<CircleShape>(body, 32);
    playerShape->setDensity(1);
    playerShape->setElasticity(1);
    playerShape->setCollisionType(Player::collisionType);

    gameScene.getGameSpace().add(body);
    gameScene.getGameSpace().add(playerShape);

    body->setMoment(std::numeric_limits<cpFloat>::infinity());

    gameScene.getGameSpace().addCollisionHandler(Player::collisionType, Room::groundTerrainCollisionType,
        [](Arbiter, Space&) { return true; },
        [=](Arbiter, Space&) { lastGroundTime = curTime; return true; },
        [](Arbiter, Space&) {}, [=](Arbiter, Space&) {});
}

Player::~Player()
{
}

void Player::update(std::chrono::steady_clock::time_point curTime)
{
    this->curTime = curTime;
    const auto& controller = gameScene.getPlayerController();
    
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
    body->applyForceAtWorldPoint(base * body->getMass() * 480, pos);

    if (controller.hasJumpAction()) // jump
        if (curTime - lastGroundTime <= 4 * UpdateFrequency)
        {
            lastGroundTime = decltype(lastGroundTime)();
            jump();
        }

    sprite.rotate(body->getVelocity().x/32);
}

void Player::jump()
{
    auto body = playerShape->getBody();
    auto pos = body->getPosition();

    auto y = -600.0 - body->getVelocity().y;
    body->applyImpulseAtWorldPoint(cpVect{0, y} * body->getMass(), pos);
}

void Player::render(Renderer& renderer)
{
    renderer.pushTransform();

    renderer.currentTransform.translate(getDisplayPosition());
    renderer.currentTransform.rotate(radiansToDegrees(playerShape->getBody()->getAngle()));

    sprite.setColor(curTime - lastGroundTime <= 4 * UpdateFrequency ? sf::Color::Red : sf::Color::White);

    renderer.pushDrawable(sprite, {}, 2);
    renderer.popTransform();
}

