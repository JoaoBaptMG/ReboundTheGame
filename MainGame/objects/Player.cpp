#include "Player.hpp"

#include "utility/vector_math.hpp"
#include "input/PlayerController.hpp"
#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "utility/chronoUtils.hpp"
#include "utility/assert.hpp"
#include "utility/vector_math.hpp"

#include <functional>
#include <limits>
#include <cppmunk/Body.h>
#include <cppmunk/CircleShape.h>
#include <cppmunk/Space.h>
#include <cppmunk/Arbiter.h>

using namespace std::literals::chrono_literals;

constexpr float MaxHorSpeed = 400;
constexpr float HorAcceleration = 600;
constexpr float PeakJumpSpeed = 660;
constexpr float DecayJumpSpeed = 360;

Player::Player(GameScene& scene)
    : abilityLevel(1), angle(0), GameObject(scene),
    sprite(scene.getResourceManager().load<sf::Texture>("player.png"))
{
    isPersistent = true;

    setName("player");
}

bool Player::configure(const Player::ConfigStruct& config)
{
    bool cfg = gameScene.getObjectByName("player") == nullptr;
    
    if (cfg)
    {
        setupPhysics();
        playerShape->getBody()->setPosition(cpVect{(cpFloat)config.position.x, (cpFloat)config.position.y});
    }
    
    return cfg;
}

void Player::setupPhysics()
{
    using namespace Chipmunk;
    
    auto body = std::make_shared<Body>(0.0, 0.0);
    playerShape = std::make_shared<CircleShape>(body, 32);
    playerShape->setDensity(1);
    playerShape->setElasticity(1);
    playerShape->setCollisionType(Player::CollisionType);

    gameScene.getGameSpace().add(body);
    gameScene.getGameSpace().add(playerShape);

    body->setMoment(std::numeric_limits<cpFloat>::infinity());

    gameScene.getGameSpace().addCollisionHandler(Player::CollisionType, Room::GroundTerrainCollisionType,
        [](Arbiter, Space&) { return true; },
        [=](Arbiter, Space&) { lastGroundTime = curTime; wallJumpPressedBefore = false; return true; },
        [](Arbiter, Space&) {}, [=](Arbiter, Space&) {});

    gameScene.getGameSpace().addCollisionHandler(Player::CollisionType, Room::WallTerrainCollisionType,
        [](Arbiter, Space&) { return true; },
        [=](Arbiter arbiter, Space& space)
        {
            if (abilityLevel >= 1)
            {
                if (wallJumpPressedBefore)
                {
                    if (curTime - wallJumpTriggerTime < 4 * UpdateFrequency)
                    {
                        wallJumpTriggerTime = decltype(wallJumpTriggerTime)();
                        wallJumpPressedBefore = false;

                        space.addPostStepCallback(nullptr, [=] { wallJump(); });
                    }
                }
                else wallJumpTriggerTime = curTime;
            }
            
            return true;
        },
        [](Arbiter, Space&) {}, [](Arbiter, Space&) {});
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
    auto dest = MaxHorSpeed * vec.x;

    if (std::abs(vel.x - dest) < 6.0)
    {
        vel.x = dest;
        body->setVelocity(vel);
    }

    auto base = cpVect{vel.x < dest ? 1.0 : vel.x > dest ? -1.0 : 0.0, 0.0};

    auto pos = body->getPosition();
    body->applyForceAtWorldPoint(base * body->getMass() * HorAcceleration, pos);

    if (onGround()) // jump
    {
        if (controller.isJumpPressed())
        {
            lastGroundTime = decltype(lastGroundTime)();
            jump();
        }
    }
    else
    {
        if (controller.isJumpReleased())
            decayJump();
        else if (controller.isJumpPressed())
        {
            if (abilityLevel >= 1)
            {
                if (!wallJumpPressedBefore && curTime - wallJumpTriggerTime < 4 * UpdateFrequency)
                {
                    wallJumpTriggerTime = decltype(wallJumpTriggerTime)();
                    wallJump();
                }
                else
                {
                    wallJumpPressedBefore = true;
                    wallJumpTriggerTime = curTime;
                }
            }
        }
    }

    angle += radiansToDegrees(body->getVelocity().x * toSeconds<float>(UpdateFrequency) / 32);
    angle -= 360 * roundf(angle/360);
}

void Player::jump()
{
    auto body = playerShape->getBody();
    auto y = -PeakJumpSpeed - body->getVelocity().y;
    body->applyImpulseAtLocalPoint(cpVect{0, y} * body->getMass(), { 0, 0 });
}

void Player::decayJump()
{
    auto body = playerShape->getBody();
    auto y = std::max(-DecayJumpSpeed - body->getVelocity().y, 0.0);
    body->applyImpulseAtLocalPoint(cpVect{0, y} * body->getMass(), { 0, 0 });
}

void Player::wallJump()
{
    auto body = playerShape->getBody();
    auto sgn = body->getVelocity().x > 0 ? 1.0 : body->getVelocity().x < 0 ? -1.0 : 0.0;
    if (sgn == 0.0) return;
    auto dv = cpVect{ sgn * MaxHorSpeed, -PeakJumpSpeed } - body->getVelocity();
    body->applyImpulseAtLocalPoint(dv * body->getMass(), { 0, 0 });
}

bool Player::onGround() const
{
     return curTime - lastGroundTime <= 7 * UpdateFrequency;
}

void Player::render(Renderer& renderer)
{
    renderer.pushTransform();

    renderer.currentTransform.translate(getDisplayPosition());
    renderer.currentTransform.rotate(angle);
    renderer.pushDrawable(sprite, {}, 2);
    renderer.popTransform();
}

