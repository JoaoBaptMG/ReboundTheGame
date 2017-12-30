#include "PlayerDeath.hpp"

#include "Player.hpp"
#include "rendering/Renderer.hpp"
#include "defaults.hpp"
#include <chronoUtils.hpp>

#include "particles/ParticleBatch.hpp"
#include "scene/GameScene.hpp"

constexpr auto FadeDuration = 30 * UpdateFrequency;
constexpr auto TotalDuration = 200 * UpdateFrequency;

PlayerDeath::PlayerDeath(GameScene& scene, Player& player, const std::shared_ptr<sf::Texture>& texture)
    : GameObject(scene), playerSprite(texture), position(player.getDisplayPosition()), spawnedParticle(false)
{
    
}

void PlayerDeath::update(std::chrono::steady_clock::time_point curTime)
{
    if (initTime == decltype(initTime)()) initTime = curTime;
    
    auto duration = curTime - initTime;
    
    if (duration <= FadeDuration)
    {
        float factor = toSeconds<float>(duration) / toSeconds<float>(FadeDuration);
        playerSprite.setFlashColor(sf::Color(255, 255, 255, 255 * factor));
    }
    else if (duration <= 2 * FadeDuration)
    {
        if (!spawnedParticle)
        {
            auto batch = std::make_unique<ParticleBatch>(gameScene, "player-particles.pe", "player-death", (size_t)32);
            batch->setPosition(position);
            gameScene.addObject(std::move(batch));
            
            spawnedParticle = true;
        }
        
        float factor = toSeconds<float>(duration - FadeDuration) / toSeconds<float>(FadeDuration);
        playerSprite.setOpacity(1 - factor);
    }
    else playerSprite.setFlashColor(sf::Color::White);
    
    if (duration > TotalDuration) gameScene.requestLevelReload();
}

void PlayerDeath::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(position);
    renderer.pushDrawable(playerSprite, {}, 25);
    renderer.popTransform();
}
