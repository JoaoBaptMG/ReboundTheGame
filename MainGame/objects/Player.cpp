#include "Player.hpp"

#include "input/PlayerController.hpp"
#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include <chronoUtils.hpp>
#include <assert.hpp>
#include <vector_math.hpp>
#include "objects/Bomb.hpp"
#include "particles/ParticleBatch.hpp"

#include <functional>
#include <limits>
#include <iostream>
#include <cppmunk/Body.h>
#include <cppmunk/CircleShape.h>
#include <cppmunk/Space.h>
#include <cppmunk/Arbiter.h>

#include <random>

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

constexpr float HardballAirFactor = 0.125;
constexpr float HardballWaterFactor = 0.75;

constexpr auto DashInterval = 40 * UpdateFrequency;
constexpr auto DashIntervalEnhanced = 90 * UpdateFrequency;
constexpr auto HardballChangeTime = 40 * UpdateFrequency;
constexpr auto GrappleFade = 30 * UpdateFrequency;

constexpr size_t BaseHealth = 208;
constexpr size_t HealthIncr = 8;

Player::Player(GameScene& scene)
    : abilityLevel(0), angle(0), lastFade(0), GameObject(scene), health(BaseHealth), maxHealth(BaseHealth),
    dashDirection(DashDir::None), dashConsumed(false), doubleJumpConsumed(false), waterArea(0),
    chargingForHardball(false), hardballEnabled(false), grappleEnabled(false), grapplePoints(0),
    dashBatch(nullptr), hardballBatch(nullptr),
    sprite(scene.getResourceManager().load<sf::Texture>("player.png")),
    grappleSprite(scene.getResourceManager().load<sf::Texture>("player-grapple.png"))
{
    isPersistent = true;

    grappleSprite.setOpacity(0);
	//upgradeToAbilityLevel(7);
    setName("player");
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
    
    auto vec = controller.movement.getValue();
    if (chargingForHardball) vec.x = vec.y = 0;
    
    auto body = playerShape->getBody();
    auto pos = body->getPosition();
    auto vel = body->getVelocity();
    auto dt = toSeconds<cpFloat>(UpdateFrequency);

    cpVect base;
    if (grapplePoints == 0)
    {
        auto dest = (abilityLevel >= 6 ? MaxHorSpeedEnhanced : MaxHorSpeed) * vec.x * hardballFactor();
        if (std::abs(vel.x - dest) < 6.0)
        {
            vel.x = dest;
            body->setVelocity(vel);
        }
        
        base = cpVect{vel.x < dest ? 1.0 : vel.x > dest ? -1.0 : 0.0, 0.0};
    }
    else base.x = vec.x, base.y = vec.y;
    body->applyForceAtLocalPoint(base * body->getMass() * HorAcceleration, cpvzero);

    if (dashDirection == DashDir::Up) angle += radiansToDegrees(vel.y * dt / 32);
    angle += radiansToDegrees(vel.x * dt / 32);
    angle -= 360 * roundf(angle/360);

    bool onGround = false, wallHit = false, onWaterCeiling = false;

    body->eachArbiter([&,this] (cp::Arbiter arbiter)
    {
        cpFloat angle = cpvtoangle(arbiter.getNormal());
        if (fabs(angle - 1.57079632679) < 0.52) onGround = true;
        if (fabs(angle) < 0.06 || fabs(angle - 3.14159265359) < 0.06 || fabs(angle + 3.14159265359) < 0.06)
            wallHit = true;
        if (onWaterNoHardball() && fabs(angle + 1.57079632679) < 0.06) onWaterCeiling = true;

        if (!cpShapeGetSensor(arbiter.getShapeA()) && !cpShapeGetSensor(arbiter.getShapeB()))
        {
            if (isDashing())
            {
                auto shp = cpShapeGetCollisionType(arbiter.getShapeA()) == CollisionType ?
                    arbiter.getShapeB() : arbiter.getShapeA();

                if (cpShapeGetCollisionType(shp) == Interactable)
                    (*(GameObject::InteractionHandler*)cpShapeGetUserData(shp))(DashInteractionType, (void*)this);
            }
            
            reset(dashTime);
        }
    });

	if (abilityLevel >= 7 && (onGround || onWaterCeiling))
        if (cpvlengthsq(vel) < 36)
            observeHardballTrigger();
    
    if (onGround)
    {
        wallJumpPressedBefore = false;
        dashConsumed = false;
        doubleJumpConsumed = false;
        if (controller.jump.isTriggered() && !onWaterNoHardball() && !chargingForHardball) jump();
    }
    else
    {
        if (controller.jump.isReleased()) decayJump();
        else if (controller.jump.isTriggered())
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

            if (abilityLevel >= 4 && !doubleJumpConsumed && (hardballEnabled == onWater()))
            {
                jump();
                doubleJumpConsumed = true;
            }
        }

        if (abilityLevel >= 3 && !hardballOnAir())
        {
            if (!dashConsumed && controller.dash.isTriggered())
            {
                if (vec.x > 0.25) dashDirection = DashDir::Right;
                else if (vec.x < -0.25) dashDirection = DashDir::Left;
                else if (vec.y < -0.25 && abilityLevel >= 10) dashDirection = DashDir::Up;

                if (dashDirection != DashDir::None) dashTime = curTime;
                dashConsumed = true;
            }
            else if (controller.dash.isReleased())
            {
                 reset(dashTime);
                 dashDirection = DashDir::None;
            }

            if (isDashing())
            {
                dash();
                
                if (!dashBatch)
                {
                    auto batch = std::make_unique<ParticleBatch>(gameScene, "player-particles.pe",
                        getDashEmitterName(), true);
                    dashBatch = batch.get();
                    gameScene.addObject(std::move(batch));
                }
            }
            if (!isDashing() && dashBatch)
            {
                dashBatch->abort();
                dashBatch = nullptr;
            }
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

    if (controller.bomb.isTriggered() && abilityLevel >= 5) lieBomb(curTime);

    if (onWater())
    {
        float density = hardballEnabled ? 0.4 : 1.6;
        
        // buoyancy
        auto force = -gameScene.getGameSpace().getGravity() * density * waterArea;
        body->applyForceAtLocalPoint(force, cpvzero);

        // drag: this velocity minimum is required for double jumps to work on water
        if (cpvlengthsq(body->getVelocity()) >= 18)
        {
            auto damping = exp(-2 * density * dt * (waterArea/PlayerArea));
            body->applyImpulseAtLocalPoint(body->getVelocity() * (damping-1) * PlayerArea, cpvzero);
        }

        if (!hardballEnabled && canWaterJump())
        {
            wallJumpPressedBefore = false;
            dashConsumed = false;
            doubleJumpConsumed = false;
            if (controller.jump.isTriggered()) jump();
        }
    }

    if (abilityLevel >= 9 && (hardballEnabled == onWater()))
    {
        if (controller.jump.isTriggered()) grappleEnabled = true;
        else if (controller.jump.isReleased()) grappleEnabled = false;
    }

    if (dashBatch) dashBatch->setPosition(getDisplayPosition());
    if (hardballBatch) hardballBatch->setPosition(getDisplayPosition());
}

void Player::jump()
{
    auto batch = std::make_unique<ParticleBatch>(gameScene, "player-particles.pe", "jump");
    batch->setPosition(getDisplayPosition());
    gameScene.addObject(std::move(batch));
    
    auto body = playerShape->getBody();
    auto dest = abilityLevel >= 6 ? PeakJumpSpeedEnhanced : PeakJumpSpeed;
    auto y = -(dest * sqrt(hardballFactor())) - body->getVelocity().y;
    body->applyImpulseAtLocalPoint(cpVect{0, y} * body->getMass(), cpvzero);
}

void Player::decayJump()
{
    auto body = playerShape->getBody();
    auto y = std::max(-(DecayJumpSpeed * sqrt(hardballFactor())) - body->getVelocity().y, 0.0);
    body->applyImpulseAtLocalPoint(cpVect{0, y} * body->getMass(), cpvzero);
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
    auto dv = cpVect{sgn*maxHorSpeed*hardballFactor(), -peakJumpSpeed*sqrt(hardballFactor())} - body->getVelocity();
    body->applyImpulseAtLocalPoint(dv * body->getMass(), cpvzero);

    auto name = body->getVelocity().x > 0 ? "wall-jump-left" : "wall-jump-right";
    auto batch = std::make_unique<ParticleBatch>(gameScene, "player-particles.pe", name);
    batch->setPosition(getDisplayPosition());
    gameScene.addObject(std::move(batch));
}

std::string Player::getDashEmitterName() const
{
    switch (dashDirection)
    {
        case DashDir::Left: return abilityLevel >= 10 ? "enhanced-dash-left" : "dash-left";
        case DashDir::Right: return abilityLevel >= 10 ? "enhanced-dash-right" : "dash-right";
        case DashDir::Up: return "enhanced-dash-up";
        default: return "";
    }
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
    body->applyImpulseAtLocalPoint(dv * body->getMass(), cpvzero);
}

void Player::lieBomb(std::chrono::steady_clock::time_point curTime)
{
    gameScene.addObject(std::make_unique<Bomb>(gameScene, getPosition(), curTime));
}

void Player::observeHardballTrigger()
{
    const auto& controller = gameScene.getPlayerController();
    auto vec = controller.movement.getValue();

    if (controller.dash.isTriggered() && vec.y > 0.5)
    {
        chargingForHardball = true;
        hardballTime = curTime;
    }
    else if (controller.dash.isReleased())
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
        else if (curTime - hardballTime >= HardballChangeTime)
        {
            reset(hardballTime);
            hardballEnabled = !hardballEnabled;
            chargingForHardball = false;
            setHardballSprite();
        }
    }

    if (chargingForHardball && !hardballBatch)
    {
        auto batch = std::make_unique<ParticleBatch>(gameScene, "player-particles.pe", "hardball-spark");
        hardballBatch = batch.get();
        gameScene.addObject(std::move(batch));
    }
    else if (!chargingForHardball && hardballBatch)
    {
        hardballBatch->abort();
        hardballBatch = nullptr;
    }
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
    return waterArea > 0.0011 * PlayerArea;
}

bool Player::onWaterNoHardball() const
{
    return !hardballEnabled && onWater();
}

bool Player::canWaterJump() const
{
    return !hardballEnabled && waterArea < 0.7 * PlayerArea;
}

bool Player::hardballOnAir() const
{
    return hardballEnabled && !onWater();
}

float Player::hardballFactor() const
{
    return hardballEnabled ? (onWater() ? HardballWaterFactor : HardballAirFactor) : 1;
}

bool Player::isDashing() const
{
    auto dashInterval = abilityLevel >= 10 ? DashIntervalEnhanced : DashInterval;
    return curTime - dashTime < dashInterval;
}

void Player::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(getDisplayPosition());
    renderer.currentTransform.rotate(angle);

    if (chargingForHardball)
    {
        auto flash = toSeconds<float>(curTime - hardballTime) / toSeconds<float>(HardballChangeTime);
        sprite.setFlashColor(sf::Color(255, 255, 255, 255 * flash));
    }
    else sprite.setFlashColor(sf::Color(255, 255, 255, 0));

    auto fade = std::min(toSeconds<float>(curTime - grappleTime) / toSeconds<float>(GrappleFade), 1.0f);
    if (grapplePoints == 0) fade = std::min(lastFade, 1.0f - fade);
    else fade = std::max(lastFade, fade);
    lastFade = fade;
    grappleSprite.setOpacity(fade);

    renderer.pushDrawable(sprite, {}, 16);
    if (fade != 0.0f) renderer.pushDrawable(grappleSprite, {}, 14);
    
    renderer.popTransform();
}

