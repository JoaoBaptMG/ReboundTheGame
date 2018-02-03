#pragma once

#include "objects/GameObject.hpp"
#include "drawables/Sprite.hpp"

#include <SFML/Graphics.hpp>
#include <cppmunk/Shape.h>
#include <cppmunk/Body.h>
#include <chrono>
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
    
    using TimePoint = std::chrono::steady_clock::time_point;
    
    Sprite sprite, grappleSprite;
    std::shared_ptr<cp::Shape> playerShape;
    ParticleBatch* dashBatch;
    ParticleBatch* hardballBatch;

    CollisionState previousWallState;

    cpVect graphicalDisplacement;
    cpFloat angle, lastFade;
    bool dashConsumed, doubleJumpConsumed;
    bool chargingForHardball, hardballEnabled, grappleEnabled;
    
    TimePoint wallJumpTriggerTime, dashTime, hardballTime,
         grappleTime, spikeTime, invincibilityTime, curTime;

    enum class DashDir { None, Left, Right, Up } dashDirection;

    size_t abilityLevel, grapplePoints;
    size_t health, maxHealth, numBombs;
    cpFloat waterArea;

    struct { cpVect pos; size_t roomID; bool hardball; } lastSafeEntries[8];
    size_t curEntry;

public:
    Player(GameScene &scene);
    virtual ~Player();

    void setupPhysics();

    virtual void update(std::chrono::steady_clock::time_point curTime) override;
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
    void lieBomb(std::chrono::steady_clock::time_point curTime);
    
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

