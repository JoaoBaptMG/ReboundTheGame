#pragma once

#include "GameObject.hpp"
#include "drawables/Sprite.hpp"
#include <chrono>

class Player;
class GameScene;

class PlayerDeath final : public GameObject
{
    std::chrono::steady_clock::time_point initTime;
    Sprite playerSprite;
    sf::Vector2f position;
    bool spawnedParticle;
    
public:
    PlayerDeath(GameScene& scene, Player& player, const std::shared_ptr<sf::Texture>& texture);
    virtual ~PlayerDeath() {}

    virtual void update(std::chrono::steady_clock::time_point curTime) override;
    virtual void render(Renderer& renderer) override;
};
