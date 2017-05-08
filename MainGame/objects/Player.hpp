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

template <typename T, typename = std::enable_if_t<std::is_default_constructible<T>::value>>
void reset(T& val) { val = T(); }

extern uintmax_t global_AbilityLevel;

constexpr cpFloat PlayerRadius = 32;
constexpr cpFloat PlayerArea = 3.14159265359 * PlayerRadius * PlayerRadius;

class Player final : public GameObject
{
    Sprite sprite;
    std::shared_ptr<cp::Shape> playerShape;
    
    float angle;
    bool wallJumpPressedBefore, dashConsumed, doubleJumpConsumed;
    bool chargingForHardball, hardballEnabled;
    
    std::chrono::steady_clock::time_point
        wallJumpTriggerTime, dashTime, hardballTime, curTime;

    enum class DashDir { None, Left, Right, Up } dashDirection;

    uintmax_t abilityLevel;
    uintmax_t health, maxHealth;
    cpFloat waterArea;

public:
    Player(GameScene &scene);
    virtual ~Player();

    void setupPhysics();

    virtual void update(std::chrono::steady_clock::time_point curTime) override;
    virtual void render(Renderer& renderer) override;

    auto getPosition() const { return playerShape->getBody()->getPosition(); }
    void setPosition(cpVect pos) { playerShape->getBody()->setPosition(pos); }

    auto getBody() const { return playerShape->getBody(); }

    auto getDisplayPosition() const
    {
        auto vec = getPosition();
        return sf::Vector2f(std::floor(vec.x), std::floor(vec.y));
    }

    auto getHealth() const { return health; }
    auto getMaxHealth() const { return maxHealth; }

    void heal(uintmax_t amount);
    void damage(uintmax_t amount);

    auto canPushCrates() const { return abilityLevel >= 2 && !hardballOnAir(); }
    void upgradeToAbilityLevel(uintmax_t level)
    {
        if (abilityLevel < level)
            abilityLevel = level;
        global_AbilityLevel = abilityLevel;
    }

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
    void lieBomb(std::chrono::steady_clock::time_point curTime);

    void setHardballSprite();

    static constexpr cpCollisionType CollisionType = 'plyr';

#pragma pack(push, 1)
    struct ConfigStruct
    {
        sf::Vector2<int16_t> position;
    };
#pragma pack(pop)

    bool configure(const ConfigStruct& config);
};

