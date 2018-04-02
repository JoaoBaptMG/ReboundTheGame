#pragma once

#include "GameObject.hpp"
#include "drawables/Sprite.hpp"
#include <chronoUtils.hpp>

class Player;
class GameScene;

class PlayerDeath final : public GameObject
{
    FrameTime initTime;
    Sprite playerSprite;
    sf::Vector2f position;
    bool spawnedParticle;
    
public:
    PlayerDeath(GameScene& scene, Player& player, const std::shared_ptr<sf::Texture>& texture);
    virtual ~PlayerDeath() {}

    virtual void update(FrameTime curTime) override;
    virtual void render(Renderer& renderer) override;
};
