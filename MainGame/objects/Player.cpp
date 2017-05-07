#include "Player.hpp"

#include "input/PlayerController.hpp"
#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "utility/chronoUtils.hpp"
#include "utility/assert.hpp"
#include "utility/vector_math.hpp"
#include "objects/Bomb.hpp"

#include <functional>
#include <limits>
#include <iostream>
#include <cppmunk/Body.h>
#include <cppmunk/CircleShape.h>
#include <cppmunk/Space.h>
#include <cppmunk/Arbiter.h>

using namespace std::literals::chrono_literals;

size_t global_AbilityLevel = 0; // TODO: factor this global out

constexpr float MaxHorSpeed = 400;
constexpr float MaxHorSpeedEnhanced = 560;
constexpr float HorAcceleration = 800;

constexpr float PeakJumpSpeed = 480;
constexpr float PeakJumpSpeedEnhanced = 600;
constexpr float DecayJumpSpeed = 300;

constexpr float DashSpeed = 600;
constexpr float DashSpeedEnhanced = 800;

constexpr float HardballFactor = 4;

const auto DashInterval = 40 * UpdateFrequency;
const auto DashIntervalEnhanced = 90 * UpdateFrequency;

constexpr size_t BaseHealth = 208;
constexpr size_t HealthIncr = 8;

Player::Player(GameScene& scene)
    : abilityLevel(0), angle(0), GameObject(scene), health(BaseHealth), maxHealth(BaseHealth),
    dashDirection(DashDir::None), dashConsumed(false), doubleJumpConsumed(false), waterArea(0),
    chargingForHardball(false), hardballEnabled(false), hardballShape(PlayerRadius),
    sprite(scene.getResourceManager().load<sf::Texture>("player.png"))
{
    isPersistent = true;

    setName("player");
    hardballShape.setOrigin(PlayerRadius, PlayerRadius);
    upgradeToAbilityLevel(6);
}

bool Player::configure(const Player::ConfigStruct& config)
{
    bool cfg = gameScene.getObjectByName("player") == nullptr;
    
    if (cfg)
    {
        setupPhysics();
        setPosition(cpVect{(cpFloat)config.position.x, (cpFloat)config.position.y});
    }
    
    return cfg;
}

void Player::setupPhysics()
{
    using namespace cp;
    
    auto body = std::make_shared<Body>(0.0, 0.0);
    playerShape = std::make_shared<CircleShape>(body, PlayerRadius);
    playerShape->setDensity(1);
    playerShape->setElasticity(1);
    playerShape->setCollisionType(CollisionType);

    gameScene.getGameSpace().add(body);
    gameScene.getGameSpace().add(playerShape);

    body->setMoment(std::numeric_limits<cpFloat>::infinity());
    body->setUserData((void*)this);

    gameScene.getGameSpace().addWildcardCollisionHandler(CollisionType,
        [] (Arbiter, Space&) { return true; },
        [] (Arbiter arbiter, Space&)
        {
            cpFloat angle = cpvtoangle(arbiter.getNormal());
            if (fabs(angle - 1.57079632679) < 0.52)
                arbiter.setRestitution(0);
            return true;
        }, [] (Arbiter, Space&) {}, [] (Arbiter, Space&) {});

    printf("mass = %g\n", body->getMass());
}

Player::~Player()
{
    if (playerShape)
    {
        auto body = playerShape->getBody();
        gameScene.getGameSpace().remove(playerShape);
        gameScene.getGameSpace().remove(body);
    }
}

void Player::update(std::chrono::steady_clock::time_point curTime)
{
    this->curTime = curTime;
    const auto& controller = gameScene.getPlayerController();
    
    auto vec = controller.getMovementVector();
    auto body = playerShape->getBody();

    auto vel = body->getVelocity();
    auto dest = (abilityLevel >= 6 ? MaxHorSpeedEnhanced : MaxHorSpeed) * vec.x;
    if (hardballOnAir()) dest /= HardballFactor;

    if (std::abs(vel.x - dest) < 6.0)
    {
        vel.x = dest;
        body->setVelocity(vel);
    }

    auto base = cpVect{vel.x < dest ? 1.0 : vel.x > dest ? -1.0 : 0.0, 0.0};

    auto pos = body->getPosition();
    body->applyForceAtWorldPoint(base * body->getMass() * HorAcceleration, pos);

    bool onGround = false, wallHit = false;

    body->eachArbiter([&,this] (cp::Arbiter arbiter)
    {
        cpFloat angle = cpvtoangle(arbiter.getNormal());
        if (fabs(angle - 1.57079632679) < 0.52) onGround = true;
        if (fabs(angle) < 0.06 || fabs(angle - 3.14159265359) < 0.06)
            wallHit = true;

        if (!cpShapeGetSensor(arbiter.getShapeA()) && !cpShapeGetSensor(arbiter.getShapeB()))
            reset(dashTime);
    });

    if (onGround)
    {
        wallJumpPressedBefore = false;
        dashConsumed = false;
        doubleJumpConsumed = false;
        if (controller.isJumpPressed() && !onWater()) jump();
        
        if (abilityLevel >= 7)
        {
            if (controller.isDashPressed() && vec.y > 0.5)
            {
                chargingForHardball = true;
                hardballTime = curTime;
            }
            else if (controller.isDashReleased())
            {
                chargingForHardball = false;
                reset(hardballTime);
            }

            if (chargingForHardball)
            {
                if (vec.y <= 0.5)
                {
                    reset(hardballTime);
                    chargingForHardball = false;
                }
                else if (curTime - hardballTime >= 40 * UpdateFrequency)
                {
                    reset(hardballTime);
                    hardballEnabled = !hardballEnabled;
                    chargingForHardball = false;
                    setHardballSprite();
                }
            }
        }
    }
    else
    {
        if (controller.isJumpReleased()) decayJump();
        else if (controller.isJumpPressed())
        {
            if (abilityLevel >= 1 && !hardballOnAir())
            {
                if (!wallJumpPressedBefore && curTime - wallJumpTriggerTime < 4 * UpdateFrequency)
                    wallJump();
                else
                {
                    wallJumpPressedBefore = true;
                    wallJumpTriggerTime = curTime;
                }
            }

            if (abilityLevel >= 4 && !doubleJumpConsumed && !onWater() && !hardballOnAir())
            {
                jump();
                doubleJumpConsumed = true;
            }
        }

        if (abilityLevel >= 3 && !hardballOnAir())
        {
            if (!dashConsumed && controller.isDashPressed())
            {
                if (vec.x > 0.25) dashDirection = DashDir::Right;
                else if (vec.x < -0.25) dashDirection = DashDir::Left;
                else if (vec.y < -0.25 && abilityLevel >= 10) dashDirection = DashDir::Up;

                if (dashDirection != DashDir::None) dashTime = curTime;
            }
            else if (controller.isDashReleased())
            {
                 reset(dashTime);
                 dashDirection = DashDir::None;
                 dashConsumed = true;
            }

            auto dashInterval = abilityLevel >= 10 ? DashIntervalEnhanced : DashInterval;
            if (curTime - dashTime < dashInterval) dash();
        }
    }

    if (wallHit && abilityLevel >= 1 && !hardballOnAir())
    {
        if (wallJumpPressedBefore)
        {
            if (curTime - wallJumpTriggerTime < 4 * UpdateFrequency)
            {
                wallJumpPressedBefore = false;
                wallJump();
            }
        }
        else wallJumpTriggerTime = curTime;
    }

    if (controller.isBombPressed() && abilityLevel >= 5) lieBomb(curTime);

    if (onWater())
    {
        // buoyancy
        auto force = -gameScene.getGameSpace().getGravity() * 1.6 * waterArea;
        body->applyImpulseAtLocalPoint(force * toSeconds<cpFloat>(UpdateFrequency), { 0, 0 });

        //drag
        auto damping = exp(-3.3 * toSeconds<cpFloat>(UpdateFrequency) * (waterArea/PlayerArea));
        body->applyImpulseAtLocalPoint(body->getVelocity() * (damping-1) * PlayerArea, { 0, 0 });

        if (controller.isJumpPressed() && canWaterJump()) jump();
    }

    angle += radiansToDegrees(body->getVelocity().x * toSeconds<float>(UpdateFrequency) / 32);
    angle -= 360 * roundf(angle/360);
}

void Player::jump()
{
    auto body = playerShape->getBody();
    auto dest = abilityLevel >= 6 ? PeakJumpSpeedEnhanced : PeakJumpSpeed;
    if (hardballOnAir()) dest /= sqrt(HardballFactor);
    auto y = -dest - body->getVelocity().y;
    body->applyImpulseAtLocalPoint(cpVect{0, y} * body->getMass(), { 0, 0 });
}

void Player::decayJump()
{
    auto body = playerShape->getBody();
    auto dest = DecayJumpSpeed;
    if (hardballOnAir()) dest /= HardballFactor;
    auto y = std::max(-dest - body->getVelocity().y, 0.0);
    body->applyImpulseAtLocalPoint(cpVect{0, y} * body->getMass(), cpVect{0, 0});
}

void Player::wallJump()
{
    reset(wallJumpTriggerTime);
    doubleJumpConsumed = false;

    auto maxHorSpeed = abilityLevel >= 6 ? MaxHorSpeedEnhanced : MaxHorSpeed;
    auto peakJumpSpeed = abilityLevel >= 6 ? PeakJumpSpeedEnhanced : PeakJumpSpeed;
    
    auto body = playerShape->getBody();
    auto sgn = body->getVelocity().x > 0 ? 1.0 : body->getVelocity().x < 0 ? -1.0 : 0.0;
    if (sgn == 0.0) return;
    auto dv = cpVect{sgn * maxHorSpeed, -peakJumpSpeed} - body->getVelocity();
    body->applyImpulseAtLocalPoint(dv * body->getMass(), cpVect{0, 0});
}

void Player::dash()
{
    auto body = playerShape->getBody();
    cpVect vel{0, 0};

    auto dashSpeed = abilityLevel >= 10 ? DashSpeedEnhanced : DashSpeed;

    switch (dashDirection)
    {
        case DashDir::Left: vel.x = -dashSpeed; break;
        case DashDir::Right: vel.x = +dashSpeed; break;
        case DashDir::Up: if (abilityLevel >= 10) vel.y = -dashSpeed; break;
        default: break;
    }

    auto dv = vel - body->getVelocity();
    body->applyImpulseAtLocalPoint(dv * body->getMass(), cpVect{0, 0});
}

void Player::lieBomb(std::chrono::steady_clock::time_point curTime)
{
    auto bomb = std::make_unique<Bomb>(gameScene, getPosition(), curTime);
    gameScene.addObject(std::move(bomb));
}

void Player::setHardballSprite()
{
    auto name = hardballEnabled ? "player-hard.png" : "player.png";
    sprite.setTexture(gameScene.getResourceManager().load<sf::Texture>(name));
}

void Player::heal(size_t amount)
{
    health += amount;
    if (health > maxHealth) health = maxHealth;
}

void Player::damage(size_t amount)
{
    if (health <= amount) health = 0;
    else health -= amount;
}

bool Player::onWater() const
{
    return !hardballEnabled && waterArea > 0;
}

bool Player::canWaterJump() const
{
    return waterArea < 0.7 * PlayerArea;
}

bool Player::hardballOnAir() const
{
    return waterArea <= 0 && hardballEnabled;
}

void Player::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(getDisplayPosition());
    renderer.currentTransform.rotate(angle);
    renderer.pushDrawable(sprite, {}, 16);

    if (chargingForHardball)
    {
        auto blend = toSeconds<float>(curTime - hardballTime) / toSeconds<float>(40 * UpdateFrequency);
        hardballShape.setFillColor(sf::Color(255, 255, 255, 255 * blend));
        renderer.pushDrawable(hardballShape, {}, 17);
    }
    
    renderer.popTransform();
}

