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

#pragma once

#include "objects/GameObject.hpp"
#include "drawables/Sprite.hpp"
#include "gameplay/Script.hpp"

#include <SFML/Graphics.hpp>
#include <cppmunk/Shape.h>
#include <cppmunk/Body.h>
#include <chronoUtils.hpp>
#include <chronoUtils.hpp>
#include <cmath>

#include <type_traits>

class PlayerController;
class ResourceManager;
class GameScene;
class Renderer;
class ParticleBatch;
class Bomb;
class GUI;
namespace collectibles
{
    class Powerup;
}

constexpr size_t MaxBombs = 4;

constexpr cpFloat PlayerRadius = 32;
constexpr cpFloat PlayerArea = 3.14159265359 * PlayerRadius * PlayerRadius;

class Player final : public GameObject
{
    enum class CollisionState { None, Ground, WallLeft, WallRight, Ceiling, Spike };
    
    using TimePoint = FrameTime;
    
    Sprite sprite, grappleSprite;
    std::shared_ptr<cp::Shape> playerShape;
    ParticleBatch* dashBatch;
    ParticleBatch* hardballBatch;

    CollisionState previousWallState;

    cpVect graphicalDisplacement;
    cpFloat angle, lastFade;
    bool dashConsumed, doubleJumpConsumed, interactionViable;
    bool chargingForHardball, hardballEnabled, grappleEnabled;
    bool doubleArmor, moveRegen;
    
    TimePoint wallJumpTriggerTime, dashTime, hardballTime,
         grappleTime, spikeTime, invincibilityTime, curTime;

    enum class DashDir { None, Left, Right, Up } dashDirection;

    size_t abilityLevel, grapplePoints;
    size_t health, maxHealth, numBombs;
    cpFloat microHealth;
    cpFloat waterArea;

    struct { cpVect pos; size_t roomID; bool hardball; } lastSafeEntries[8];
    size_t curEntry;

public:
    Player(GameScene &scene);
    virtual ~Player();

    void setupPhysics();

    virtual void update(FrameTime curTime) override;
    virtual void render(Renderer& renderer) override;

    auto getPosition() const { return playerShape->getBody()->getPosition(); }
    void setPosition(cpVect pos) { playerShape->getBody()->setPosition(pos); }

    virtual bool notifyScreenTransition(cpVect displacement) override;

    auto getVelocity() const { return playerShape->getBody()->getVelocity(); }

    auto getBody() const { return playerShape->getBody(); }

    auto getDisplayPosition() const
    {
        auto vec = getPosition() + graphicalDisplacement;
        return sf::Vector2f((float)std::round(vec.x), (float)std::round(vec.y));
    }

    auto getHealth() const { return health; }
    auto getMaxHealth() const { return maxHealth; }

    void heal(size_t amount);
    bool damage(size_t amount, bool overrideInvincibility = false);
    void microHeal(float amount);
    bool microDamage(float amount);
    void die();

    bool canPushCrates() const { return abilityLevel >= 2 && !hardballOnAir(); }
    bool canBreakDash() const { return abilityLevel >= 8; }
    bool isEnhanced() const { return abilityLevel >= 6; }

    auto canGrapple() const { return grappleEnabled; }
    void setGrappling(bool val)
    {
        if (val) grapplePoints++;
        else grapplePoints--;
        grappleTime = curTime;
    }

    float getDashDisplay() const;
    
    void upgradeToAbilityLevel(size_t level);
    void enableDoubleArmor();
    void enableMoveRegen();
    void upgradeHealth();

    bool isDashing() const;
    std::string getDashEmitterName() const;

    bool isHardballEnabled() const { return hardballEnabled; }
    bool onWater() const;
    bool onWaterNoHardball() const;
    bool canWaterJump() const;
    bool hardballOnAir() const;
    cpFloat hardballFactor() const;
    void addToWaterArea(cpFloat area) { waterArea += area; }

    bool wantsToInteract() const;
    
    void applyMovementForces();
    void applyWaterForces();
    CollisionState enumerateAndActOnArbiters();
    
    void actOnGround(bool waterborne = false);
    void actAirborne();
    void actOnWalls(CollisionState state);
    
    bool observeWallJumpTrigger();
    void observeDashAction();
    void observeDoubleJumpAction();
    void observeBombAction();
    void observeHardballTrigger();
    void observeGrappleTrigger();
    void observePauseTrigger();

    void jump();
    void decayJump();
    void wallJump(CollisionState state);
    void dash();
    void lieBomb(FrameTime curTime);
    
    void abortDash();
    void disableDashBatch();
    void abortHardball();

    void hitSpikes();
    void respawnFromSpikes();

    void setPlayerSprite();

    friend class ::collectibles::Powerup;
    friend class Bomb;
    friend class GUI;
    static constexpr cpCollisionType CollisionType = 'plyr';
    static constexpr uint32_t DashInteractionType = 'pdsh';

#pragma pack(push, 1)
    struct ConfigStruct
    {
        sf::Vector2<int16_t> position;
    };
#pragma pack(pop)

    bool configure(const ConfigStruct& config);
};

