//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//


#include "Player.hpp"

#include "GameObjectFactory.hpp"
#include "input/InputPlayerController.hpp"
#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include <chronoUtils.hpp>
#include <assert.hpp>
#include <vector_math.hpp>
#include "objects/Bomb.hpp"
#include "particles/ParticleBatch.hpp"
#include "objects/Room.hpp"
#include "data/TileSet.hpp"
#include "particles/TextureExplosion.hpp"

#include "gameplay/ScriptedPlayerController.hpp"
#include "objects/PlayerDeath.hpp"
#include "language/KeyboardKeyName.hpp"

#include <functional>
#include <limits>
#include <iostream>
#include <cppmunk.h>

#include <random>
#include <cmath>

using namespace std::literals::chrono_literals;

constexpr cpFloat MaxHorSpeed = 400;
constexpr cpFloat MaxHorSpeedEnhanced = 560;
constexpr cpFloat HorAcceleration = 800;

constexpr cpFloat PeakJumpSpeed = 480;
constexpr cpFloat PeakJumpSpeedEnhanced = 600;
constexpr cpFloat DecayJumpSpeed = 300;

constexpr cpFloat NullSpeed = 36;

constexpr cpFloat DashSpeed = 600;
constexpr cpFloat DashSpeedEnhanced = 800;

constexpr cpFloat HardballAirFactor = 0.125;
constexpr cpFloat HardballWaterFactor = 0.75;

constexpr cpFloat MoveRegenRate = 2.5;

constexpr auto DashInterval = 40_frames;
constexpr auto DashIntervalEnhanced = 90_frames;
constexpr auto HardballChangeTime = 40_frames;
constexpr auto GrappleFade = 30_frames;

constexpr auto Invincibility = 60_frames;
constexpr auto InvincPeriod = 20_frames;

constexpr auto SpikeRespawnTime = 25_frames;
constexpr auto SpikeInvincibilityTime = 200_frames;

constexpr auto ExplosionDuration = 2 * SpikeRespawnTime;

constexpr size_t BaseHealth = 192;
constexpr size_t HealthIncr = 8;
constexpr size_t SpikeDamage = 50;

Player::Player(GameScene& scene)
    : abilityLevel(0), angle(0), lastFade(0), GameObject(scene), health(BaseHealth), maxHealth(BaseHealth),
    numBombs(MaxBombs), dashDirection(DashDir::None), dashConsumed(false), doubleJumpConsumed(false), waterArea(0),
    chargingForHardball(false), hardballEnabled(false), grappleEnabled(false), grapplePoints(0), dashBatch(nullptr),
    doubleArmor(false), moveRegen(false), hardballBatch(nullptr), curEntry(0), previousWallState(CollisionState::None),
    microHealth(0), sprite(scene.getResourceManager().load<sf::Texture>("player.png")), graphicalDisplacement(),
    grappleSprite(scene.getResourceManager().load<sf::Texture>("player-grapple.png"))
{
#warning Remove this!
    if (scene.getSavedGame().getCurLevel() == 2)
    {
        for (size_t i = 0; i < 76; i++) scene.getSavedGame().setPicket(i, true);
        for (size_t i = 100; i < 133; i++) scene.getSavedGame().setPicket(i, true);
    }

    isPersistent = true;

    grappleSprite.setOpacity(0);
	upgradeToAbilityLevel(scene.getSavedGame().getAbilityLevel());
    if (scene.getSavedGame().getDoubleArmor()) enableDoubleArmor();
    if (scene.getSavedGame().getMoveRegen()) enableMoveRegen();
    maxHealth += scene.getSavedGame().getGoldenTokenCount() * HealthIncr;
    health = maxHealth;
	setName("player");
}

bool Player::configure(const Player::ConfigStruct& config)
{
    bool cfg = gameScene.getObjectByName("player") == nullptr;
    
    if (cfg)
    {
        setupPhysics();
        setPosition(cpVect{(cpFloat)config.position.x, (cpFloat)config.position.y});
        
        for (auto& entry : lastSafeEntries)
        {
            entry.pos = getPosition();
            entry.roomID = gameScene.getCurrentRoomID();
            entry.hardball = false;
            
            curEntry = 0;
        }
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
        [] (Arbiter arbiter, Space&)
        {
            auto player = cpShapeGetCollisionType(arbiter.getShapeA()) == CollisionType ?
                          arbiter.getShapeA() : arbiter.getShapeB();
            auto shp = cpShapeGetCollisionType(arbiter.getShapeA()) == CollisionType ?
                       arbiter.getShapeB() : arbiter.getShapeA();

            if (cpShapeGetCollisionType(shp) == Room::CollisionType)
            {
                auto vel = cpBodyGetVelocity(cpShapeGetBody(player));

                switch (*(TileSet::Attribute*)cpShapeGetUserData(shp))
                {
                    case TileSet::Attribute::LeftSolid:
                    case TileSet::Attribute::LeftNoWalljump: return vel.x >= 0;
                    case TileSet::Attribute::RightSolid:
                    case TileSet::Attribute::RightNoWalljump: return vel.x <= 0;
                    case TileSet::Attribute::UpSolid: return vel.y >= 0;
                    case TileSet::Attribute::DownSolid: return vel.y <= 0;
                    default: return true;
                }
            }

            return true;
        },
        [] (Arbiter arbiter, Space&)
        {
            auto normal = arbiter.getNormal();
            if (normal.y * 0.75 >= fabs(normal.x))
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

void Player::update(FrameTime curTime)
{
    this->curTime = curTime;
    
    if (spikeTime != decltype(spikeTime)())
    {
        if (curTime > spikeTime) respawnFromSpikes();
        return;
    }
    
    applyMovementForces();
    auto state = enumerateAndActOnArbiters();
    
    switch (state)
    {
        case CollisionState::Ceiling: gameScene.playSound("player-wall.wav");
        case CollisionState::None: actAirborne(); break;
        case CollisionState::Ground: actOnGround(); break;
        case CollisionState::WallLeft: actOnWalls(state); break;
        case CollisionState::WallRight: actOnWalls(state); break;
        case CollisionState::Spike: hitSpikes(); interactionViable = false; return;
    }
    
    if (abilityLevel >= 5) observeBombAction();
    if (abilityLevel >= 9 && hardballEnabled == onWater()) observeGrappleTrigger();
    
    if (hardballEnabled != onWater()) disableDashBatch();
    
    if (onWater())
    {
        applyWaterForces();
        if (canWaterJump()) actOnGround(true);
        if (state == CollisionState::Ceiling)
            observeHardballTrigger();
    }

    if (invincibilityTime != decltype(invincibilityTime)() && curTime > invincibilityTime)
        reset(invincibilityTime);
        
    observePauseTrigger();
}

void Player::applyMovementForces()
{
    const auto& controller = gameScene.getPlayerController();
    auto vec = controller.movement().getValue();
    if (chargingForHardball) vec.x = vec.y = 0;
    
    auto body = playerShape->getBody();
    auto vel = body->getVelocity();

    cpVect base{};
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
    else 
    {
        base.x = vec.x, base.y = vec.y;
        body->applyForceAtLocalPoint(-gameScene.getGameSpace().getGravity() * body->getMass(), cpvzero);
    }
    body->applyForceAtLocalPoint(base * body->getMass() * HorAcceleration, cpvzero);

    auto dt = toSeconds<cpFloat>(UpdatePeriod);
    if (dashDirection == DashDir::Up) angle += radiansToDegrees(vel.y * dt / 32);
    angle += radiansToDegrees(vel.x * dt / 32);
    angle -= 360 * round(angle/360);
}

void Player::applyWaterForces()
{
    auto body = playerShape->getBody();
    auto vel = body->getVelocity();
    auto dt = toSeconds<cpFloat>(UpdatePeriod);
    
    cpFloat density = hardballEnabled ? 0.4 : 1.6;
    
    // buoyancy
    auto force = -gameScene.getGameSpace().getGravity() * density * waterArea;
    body->applyForceAtLocalPoint(force, cpvzero);

    // drag: this velocity minimum is required for double jumps to work on water
    if (cpvlengthsq(body->getVelocity()) >= 18)
    {
        auto damping = exp(-2 * density * dt * (waterArea/PlayerArea));
        playerShape->getBody()->applyImpulseAtLocalPoint(vel * (damping-1) * PlayerArea, cpvzero);
    }
}

Player::CollisionState Player::enumerateAndActOnArbiters()
{
    CollisionState state = CollisionState::None;

    graphicalDisplacement = cpVect{0, 0};
    playerShape->getBody()->eachArbiter([&,this] (cp::Arbiter arbiter)
    {
		enum { None, Left, Top, Right, Bottom } dir = None;

        auto normal = arbiter.getNormal();
        if (fabs(normal.x) * 0.75 >= fabs(normal.y))
            dir = normal.x > 0 ? Right : Left;
        else if (fabs(normal.y) * 0.75 >= fabs(normal.x))
            dir = normal.y > 0 ? Bottom : Top;

        bool walljump = true;
        if (dir == Bottom) state = CollisionState::Ground;
        else if (dir == Top) state = CollisionState::Ceiling;

        if (!cpShapeGetSensor(arbiter.getShapeA()) && !cpShapeGetSensor(arbiter.getShapeB()))
        {
            auto shp = cpShapeGetCollisionType(arbiter.getShapeA()) == CollisionType ?
                arbiter.getShapeB() : arbiter.getShapeA();

            if (cpShapeGetCollisionType(shp) == Room::CollisionType)
            {
                auto attr = *(TileSet::Attribute*)cpShapeGetUserData(shp);
                if (TileSet::isNoWalljump(attr)) walljump = false;
                else if (attr == TileSet::Attribute::Spike) state = CollisionState::Spike;
            }
            else abortHardball();

            cpFloat sgn = cpShapeGetCollisionType(arbiter.getShapeA()) == CollisionType ? 1 : -1;
            cpFloat depthSum = 0;

            auto set = cpArbiterGetContactPointSet(arbiter);
            for (size_t i = 0; i < set.count; i++)
                depthSum += set.points[i].distance;

            graphicalDisplacement += sgn * depthSum * set.normal;
            
            if (isDashing() && cpShapeGetCollisionType(shp) == Interactable)
                (*(GameObject::InteractionHandler*)cpShapeGetUserData(shp))(DashInteractionType, (void*)this);
            
            abortDash();
        }

		if (state == CollisionState::None && walljump && (dir == Left || dir == Right))
            state = dir == Left ? CollisionState::WallLeft : CollisionState::WallRight;
    });
    
    return state;
}

void Player::actOnGround(bool waterborne)
{
    const auto& controller = gameScene.getPlayerController();
    
    lastSafeEntries[curEntry].pos = getPosition();
    lastSafeEntries[curEntry].roomID = gameScene.getCurrentRoomID();
    lastSafeEntries[curEntry].hardball = hardballEnabled;
    curEntry = (curEntry + 1) % 8;

    previousWallState = CollisionState::None;
    dashConsumed = false;
    doubleJumpConsumed = false;
    reset(wallJumpTriggerTime);
    if (controller.jump().isTriggered() && !chargingForHardball) jump();
	if (abilityLevel >= 7) observeHardballTrigger();
    if (moveRegen && !waterborne)
    {
        cpFloat spd = fabs(playerShape->getBody()->getVelocity().x);
        float dt = toSeconds<float>(UpdatePeriod);
        if (spd > 0.08 * MaxHorSpeedEnhanced)
            microHeal(MoveRegenRate * spd * dt / MaxHorSpeedEnhanced);
    }
    
    interactionViable = controller.dash().isTriggered();
}

void Player::actAirborne()
{
    const auto& controller = gameScene.getPlayerController();
    auto vec = controller.movement().getValue();
    
    if (controller.jump().isReleased()) decayJump();
    
    if (hardballEnabled == onWater())
    {
        bool wallJumped;
        if (abilityLevel >= 1) wallJumped = observeWallJumpTrigger();
        if (abilityLevel >= 3) observeDashAction();
        if (abilityLevel >= 4 && !wallJumped) observeDoubleJumpAction();
    }
    
    bool dashImpossible = abilityLevel >= 3 ? fabsf(vec.x) < 0.5 && fabsf(vec.y) < 0.5 : true;
    interactionViable = controller.dash().isTriggered() && dashImpossible;
}

void Player::actOnWalls(Player::CollisionState state)
{
    const auto& controller = gameScene.getPlayerController();

    gameScene.playSound("player-wall.wav");

    if (abilityLevel >= 1 && hardballEnabled == onWater())
    {
        if (controller.jump().isTriggered()) wallJump(state);
        else if (previousWallState == CollisionState::None &&
            curTime - wallJumpTriggerTime < 6_frames) wallJump(state);
        else
        {
            previousWallState = state;
            wallJumpTriggerTime = curTime;
        }
    }
    
    interactionViable = false;
}

bool Player::observeWallJumpTrigger()
{
    const auto& controller = gameScene.getPlayerController();
    
    if (controller.jump().isTriggered())
    {
        if (previousWallState == CollisionState::None) wallJumpTriggerTime = curTime;
        else if (curTime - wallJumpTriggerTime < 6_frames)
        {
            wallJump(previousWallState);
            previousWallState = CollisionState::None;
            return true;
        }
    }
    
    return false;
}

void Player::observeDashAction()
{
    const auto& controller = gameScene.getPlayerController();
    auto vec = controller.movement().getValue();
    
    if (!dashConsumed && controller.dash().isTriggered())
    {
        if (vec.x > 0.25) dashDirection = DashDir::Right;
        else if (vec.x < -0.25) dashDirection = DashDir::Left;
        else if (vec.y < -0.25 && abilityLevel >= 10) dashDirection = DashDir::Up;

        if (dashDirection != DashDir::None)
        {
            dashTime = curTime;
            dashConsumed = true;
        }
    }
    else if (controller.dash().isReleased()) abortDash();

    if (isDashing())
    {
        dash();
        
        if (!dashBatch)
        {
            auto batch = std::make_unique<ParticleBatch>(gameScene, "player-particles.pe", getDashEmitterName(), true);
            dashBatch = batch.get();
            gameScene.addObject(std::move(batch));
        }
        
        dashBatch->setPosition(getDisplayPosition());
    }
    else disableDashBatch();
}

void Player::observeDoubleJumpAction()
{
    const auto& controller = gameScene.getPlayerController();
    
    if (controller.jump().isTriggered() && !doubleJumpConsumed)
    {
        jump();
        doubleJumpConsumed = true;
    }
}

void Player::observeBombAction()
{
    const auto& controller = gameScene.getPlayerController();
    
    if (controller.bomb().isTriggered() && numBombs > 0) lieBomb(curTime);
}

void Player::observeHardballTrigger()
{
    const auto& controller = gameScene.getPlayerController();
    auto vec = controller.movement().getValue();

    bool stopped = cpvlengthsq(playerShape->getBody()->getVelocity()) < NullSpeed*NullSpeed;

    if (invincibilityTime != decltype(invincibilityTime)())
    {
        chargingForHardball = false;
        reset(hardballTime);
        return;
    }

    if (controller.dash().isTriggered() && vec.y > 0.5 && stopped)
    {
        chargingForHardball = true;
        hardballTime = curTime;
    }
    else if (controller.dash().isReleased())
    {
        reset(hardballTime);
        chargingForHardball = false;
    }

    if (hardballTime != decltype(hardballTime)())
    {
        if (!hardballBatch)
        {
            auto batch = std::make_unique<ParticleBatch>(gameScene, "player-particles.pe", "hardball-spark");
            hardballBatch = batch.get();
            hardballBatch->setPosition(getDisplayPosition());
            gameScene.addObject(std::move(batch));
            gameScene.playSound("player-hardball.wav");
        }
        
        if (vec.y <= 0.5 || !stopped)
        {
            reset(hardballTime);
            chargingForHardball = false;
        }
        else if (curTime - hardballTime >= HardballChangeTime)
        {
            reset(hardballTime);
            chargingForHardball = false;
            hardballEnabled = !hardballEnabled;
            setPlayerSprite();
            
            if (hardballBatch)
            {
                hardballBatch->abort();
                hardballBatch = nullptr;
            }
        }
    }
    else
    {
        if (hardballBatch)
        {
            hardballBatch->abort();
            hardballBatch = nullptr;
        }
    }
}

void Player::observeGrappleTrigger()
{
    const auto& controller = gameScene.getPlayerController();
    
    if (controller.jump().isTriggered()) grappleEnabled = true;
    else if (controller.jump().isReleased()) grappleEnabled = false;
}

void Player::observePauseTrigger()
{
    const auto& controller = gameScene.getPlayerController();
    
    if (controller.pause().isTriggered()) gameScene.requestPauseScreen();
}

bool Player::notifyScreenTransition(cpVect displacement)
{
    setPosition(getPosition() + displacement);
    return true;
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

void Player::wallJump(Player::CollisionState state)
{
    reset(wallJumpTriggerTime);
    doubleJumpConsumed = false;

    auto maxHorSpeed = abilityLevel >= 6 ? MaxHorSpeedEnhanced : MaxHorSpeed;
    auto peakJumpSpeed = abilityLevel >= 6 ? PeakJumpSpeedEnhanced : PeakJumpSpeed;
    
    auto body = playerShape->getBody();
	auto sgn = state == CollisionState::WallRight ? -1.0 : 1.0;
    auto dv = cpVect{sgn*maxHorSpeed*hardballFactor(), -peakJumpSpeed*sqrt(hardballFactor())} - body->getVelocity();
    body->applyImpulseAtLocalPoint(dv * body->getMass(), cpvzero);

    auto name = state == CollisionState::WallLeft ? "wall-jump-left" : "wall-jump-right";
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

void Player::abortDash()
{
    reset(dashTime);
    dashDirection = DashDir::None;
}

void Player::disableDashBatch()
{
    if (dashBatch)
    {
        dashBatch->abort();
        dashBatch = nullptr;
    }
}

void Player::abortHardball()
{
    reset(hardballTime);
    chargingForHardball = false;
}

void Player::lieBomb(FrameTime curTime)
{
    numBombs--;
    gameScene.addObject(std::make_unique<Bomb>(gameScene, getPosition() + graphicalDisplacement, curTime));
}

//extern std::string CurrentIcon;
void Player::setPlayerSprite()
{
    auto name = hardballEnabled ? "player-hard.png" : abilityLevel >= 6 ? "player-enhanced.png" : "player.png";
    sprite.setTexture(gameScene.getResourceManager().load<sf::Texture>(name));
	//CurrentIcon = name;
}

void Player::hitSpikes()
{
    disableDashBatch();
    if (damage(SpikeDamage, true)) return;
    gameScene.getGameSpace().remove(playerShape->getBody());
    spikeTime = curTime + SpikeRespawnTime;

    auto grav = gameScene.getGameSpace().getGravity();
    auto displayGravity = sf::Vector2f((float)grav.x, (float)grav.y);
    
    auto explosion = std::make_unique<TextureExplosion>(gameScene, sprite.getTexture(), ExplosionDuration,
        sf::FloatRect(-32, 0, 64, 64), displayGravity, TextureExplosion::Density, 8, 8, 25);
    explosion->setPosition(getDisplayPosition());
    gameScene.addObject(std::move(explosion));

    gameScene.playSound("player-hit-spike.wav");
}

void Player::respawnFromSpikes()
{
    reset(spikeTime);
    
    const auto& entry = lastSafeEntries[(curEntry + 1) % 8];
    
    if (gameScene.getCurrentRoomID() != entry.roomID)
        gameScene.requestRoomLoad(entry.roomID);
        
    setPosition(entry.pos);
    playerShape->getBody()->setVelocity(cpvzero);
    gameScene.getGameSpace().add(playerShape->getBody());
    
    hardballEnabled = entry.hardball;
    setPlayerSprite();

    invincibilityTime = curTime + SpikeInvincibilityTime;
}

void Player::heal(size_t amount)
{
    health += amount;
    if (health > maxHealth) health = maxHealth;
}

bool Player::damage(size_t amount, bool overrideInvincibility)
{
    if (!overrideInvincibility && invincibilityTime != decltype(invincibilityTime)()) return false;

    if (!overrideInvincibility) gameScene.playSound("player-damage.wav");

    if (doubleArmor) amount /= 2;
    if (health <= amount)
    {
        die();
        return true;
    }
    else health -= amount;

    invincibilityTime = curTime + Invincibility;
    return false;
}

void Player::microHeal(float amount)
{
    microHealth += amount;

    if (microHealth >= 1)
    {
        size_t newAmount = floor(microHealth);
        microHealth -= newAmount;
        heal(newAmount);
    }
}

bool Player::microDamage(float amount)
{
    if (doubleArmor) amount /= 2;
    microHealth -= amount;

    if (microHealth < 0)
    {
        size_t newAmount = -floor(microHealth);
        microHealth += newAmount;
        if (doubleArmor) newAmount *= 2;
        return damage(newAmount, true);
    }
    return false;
}

void Player::die()
{
    health = 0;
    disableDashBatch();
    gameScene.addObject(std::make_unique<PlayerDeath>(gameScene, *this, sprite.getTexture()));
    remove();
}

void Player::upgradeToAbilityLevel(size_t level)
{
    ASSERT(level <= 10);
    if (abilityLevel < level)
    {
        abilityLevel = level;
        setPlayerSprite();
        gameScene.getSavedGame().setAbilityLevel(level);
    }
}

void Player::enableDoubleArmor()
{
    doubleArmor = true;
}

void Player::enableMoveRegen()
{
    moveRegen = true;
}

void Player::upgradeHealth()
{
    maxHealth += HealthIncr;

    gameScene.runCutsceneScript([=] (Script& script)
    {
        auto &msgbox = gameScene.getMessageBox();
        auto tokens = gameScene.getSavedGame().getGoldenTokenCount();

        if (tokens == 1)
            msgbox.displayFormattedString(script, "msg-golden-token-collect1", {},
                {{"incr", HealthIncr}, {"max", 30}, {"hpmax", BaseHealth + 30 * HealthIncr}}, {});
        else if (tokens == 29) msgbox.displayFormattedString(script, "msg-golden-token-collectall-1", {}, {{"n", 29}}, {});
        else if (tokens == 30) msgbox.displayString(script, "msg-golden-token-collectall");
        else msgbox.displayFormattedString(script, "msg-golden-token-collectn", {}, {{"n", tokens}, {"mn", 30-tokens}}, {});
    });
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

cpFloat Player::hardballFactor() const
{
    return hardballEnabled ? (onWater() ? HardballWaterFactor : HardballAirFactor) : 1;
}

bool Player::isDashing() const
{
    auto dashInterval = abilityLevel >= 10 ? DashIntervalEnhanced : DashInterval;
    return curTime - dashTime < dashInterval;
}

float Player::getDashDisplay() const
{
    auto dashInterval = abilityLevel >= 10 ? DashIntervalEnhanced : DashInterval;
    
    if (!dashConsumed) return 1;
    else return std::max(1.0f - toSeconds<float>(curTime - dashTime)/toSeconds<float>(dashInterval), 0.0f);
}

bool Player::wantsToInteract() const
{
    return interactionViable;
}

void Player::render(Renderer& renderer)
{
    if (spikeTime == decltype(spikeTime)())
    {
        renderer.pushTransform();
        renderer.currentTransform.translate(getDisplayPosition());
        renderer.currentTransform.rotate((float)angle);

        if (invincibilityTime != decltype(invincibilityTime)())
        {
            auto phase = toSeconds<cpFloat>(invincibilityTime - curTime) / toSeconds<cpFloat>(InvincPeriod);
            auto amp = fabs(sin(M_PI * phase));
            sprite.setFlashColor(sf::Color(255, 0, 0, 128 * amp));
        }
        else if (chargingForHardball)
        {
            auto flash = toSeconds<float>(curTime - hardballTime) / toSeconds<float>(HardballChangeTime);
            sprite.setFlashColor(sf::Color(255, 255, 255, 255 * flash));
        }
        else sprite.setFlashColor(sf::Color(255, 255, 255, 0));

        auto fade = std::min(toSeconds<cpFloat>(curTime - grappleTime) / toSeconds<cpFloat>(GrappleFade), 1.0);
        if (grapplePoints == 0) fade = std::min(lastFade, 1.0 - fade);
        else fade = std::max(lastFade, fade);
        lastFade = fade;
        grappleSprite.setOpacity(fade);

        renderer.pushDrawable(sprite, {}, 16);
        if (fade != 0.0f) renderer.pushDrawable(grappleSprite, {}, 14);
        
        renderer.popTransform();
    }
}

REGISTER_GAME_OBJECT(Player);
