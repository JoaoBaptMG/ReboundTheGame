#pragma once

#include "objects/GameObject.hpp"
#include "drawables/Sprite.hpp"

#include <SFML/Graphics.hpp>
#include <cppmunk/Shape.h>
#include <cppmunk/Body.h>
#include <chrono>

#include <type_traits>

class PlayerController;
class ResourceManager;
class GameScene;
class Renderer;
class ParticleBatch;
class Bomb;
namespace collectibles
{
    class Powerup;
}

template <typename T, typename = std::enable_if_t<std::is_default_constructible<T>::value>>
void reset(T& val) { val = T(); }

constexpr cpFloat PlayerRadius = 32;
constexpr cpFloat PlayerArea = 3.14159265359 * PlayerRadius * PlayerRadius;

class Player final : public GameObject
{
    using TimePoint = std::chrono::steady_clock::time_point;
    
    Sprite sprite, grappleSprite;
    std::shared_ptr<cp::Shape> playerShape;
    ParticleBatch* dashBatch;
    ParticleBatch* hardballBatch;

    float angle, lastFade;
    bool wallJumpPressedBefore, dashConsumed, doubleJumpConsumed;
    bool chargingForHardball, hardballEnabled;
    bool grappleEnabled;
    
    TimePoint wallJumpTriggerTime, dashTime, hardballTime,
         grappleTime, spikeTime, invincibilityTime, curTime;

    enum class DashDir { None, Left, Right, Up } dashDirection;

    size_t abilityLevel, grapplePoints;
    size_t health, maxHealth, numBombs;
    cpFloat waterArea;

    cpVect lastSafePosition;
    size_t lastSafeRoomID;

public:
    Player(GameScene &scene);
    virtual ~Player();

    void setupPhysics();

    virtual void update(std::chrono::steady_clock::time_point curTime) override;
    virtual void render(Renderer& renderer) override;

    auto getPosition() const { return playerShape->getBody()->getPosition(); }
    void setPosition(cpVect pos) { playerShape->getBody()->setPosition(pos); }

    auto getVelocity() const { return playerShape->getBody()->getVelocity(); }

    auto getBody() const { return playerShape->getBody(); }

    auto getDisplayPosition() const
    {
        auto vec = getPosition();
        return sf::Vector2f(std::floor(vec.x), std::floor(vec.y));
    }

    auto getHealth() const { return health; }
    auto getMaxHealth() const { return maxHealth; }

    void heal(size_t amount);
    void damage(size_t amount, bool overrideInvincibility = false);

    auto canPushCrates() const { return abilityLevel >= 2 && !hardballOnAir(); }
    auto canBreakDash() const { return abilityLevel >= 8; }
    
    void upgradeToAbilityLevel(size_t level)
    {
        if (abilityLevel < level)
            abilityLevel = level;
    }

    bool isDashing() const;
    std::string getDashEmitterName() const;

    bool onWater() const;
    bool onWaterNoHardball() const;
    bool canWaterJump() const;
    bool hardballOnAir() const;
    float hardballFactor() const;
    void addToWaterArea(cpFloat area) { waterArea += area; }

    void jump();
    void decayJump();
    void wallJump();
    void dash();
    void observeHardballTrigger();
    void lieBomb(std::chrono::steady_clock::time_point curTime);

    void hitSpikes();
    void respawnFromSpikes();

    void setHardballSprite();

    auto canGrapple() const { return grappleEnabled; }
    void setGrappling(bool val)
    {
        if (val) grapplePoints++;
        else grapplePoints--;
        grappleTime = curTime;
    }

    friend class ::collectibles::Powerup;
    friend class Bomb;
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

