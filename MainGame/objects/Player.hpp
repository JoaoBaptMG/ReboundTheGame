#pragma once

#include "objects/GameObject.hpp"

#include <SFML/Graphics.hpp>
#include <cppmunk/Shape.h>
#include <cppmunk/Body.h>
#include <chrono>

class PlayerController;
class ResourceManager;
class GameScene;
class Renderer;

class Player final : public GameObject
{
    sf::Sprite sprite;
    std::shared_ptr<Chipmunk::Shape> playerShape;

    std::chrono::steady_clock::time_point lastGroundTime;
    std::chrono::steady_clock::time_point curTime;

    size_t abilityLevel;

public:
    Player(GameScene &scene);
    ~Player();

    void setupPhysics();

    virtual void update(std::chrono::steady_clock::time_point curTime) override;
    virtual void render(Renderer& renderer) override;

    auto getPosition() const { return playerShape->getBody()->getPosition(); }

    auto getDisplayPosition() const
    {
        auto vec = getPosition();
        return sf::Vector2f(std::floor(vec.x), std::floor(vec.y));
    }

    void jump();

    static constexpr cpCollisionType collisionType = 'plyr';

#pragma pack(push, 1)
    struct ConfigStruct
    {
        sf::Vector2f position;
    };
#pragma pack(pop)

    bool configure(const ConfigStruct& config);
};

