#include "GUI.hpp"

#include <SFML/Graphics.hpp>

#include "scene/GameScene.hpp"
#include "resources/ResourceManager.hpp"
#include "rendering/Renderer.hpp"
#include "objects/Player.hpp"
#include "defaults.hpp"

GUI::GUI(GameScene& scene) : gameScene(scene),
    guiLeft(scene.getResourceManager().load<sf::Texture>("gui-left.png"), sf::Vector2f(0, 0)),
    guiRight(scene.getResourceManager().load<sf::Texture>("gui-right.png"), sf::Vector2f(0, 0)),
    playerMeter(MeterSize::Normal), dashMeter(MeterSize::Small), lastIconName(""), drawDash(false)
{
    playerMeter.setColors(sf::Color::Green, sf::Color::Red, sf::Color(80, 80, 80, 255));
    playerMeter.setPosition(4, 484);
    
    dashMeter.setColors(sf::Color(162, 0, 255, 255), sf::Color::Yellow, sf::Color(80, 80, 80, 255));
    dashMeter.setPosition(48, 116);
    dashMeter.setHeight(48);
    dashMeter.setIcon(scene.getResourceManager().load<sf::Texture>("icon-dash.png"));
    
    for (auto& sprite : bombSprites)
        sprite.setTexture(scene.getResourceManager().load<sf::Texture>("icon-bomb.png"));
}

void GUI::update(std::chrono::steady_clock::time_point curTime)
{
    auto player = gameScene.getObjectByName<Player>("player");
    
    if (player)
    {
        playerMeter.setHeight(player->getMaxHealth());
        playerMeter.setTarget(player->getHealth());
        
        dashMeter.setTarget(dashMeter.getHeight() * player->getDashDisplay());
        
        auto iconName = player->isHardballEnabled() ? "icon-player-hard.png" :
            player->isEnhanced() ? "icon-player-enhanced.png" : "icon-player.png";
            
        if (iconName != lastIconName)
        {
            playerMeter.setIcon(gameScene.getResourceManager().load<sf::Texture>(iconName));
            lastIconName = iconName;
        }
        
        auto prevSize = abilitySprites.size();
        abilitySprites.resize(player->abilityLevel);
        if (abilitySprites.size() > prevSize)
        {
            for (size_t i = prevSize; i < abilitySprites.size(); i++)
            {
                std::string name = "powerup" + std::to_string(i+1) + ".png";
                abilitySprites[i].setTexture(gameScene.getResourceManager().load<sf::Texture>(name));
                abilitySprites[i].setTextureRect(sf::FloatRect(4, 4, 40, 40));
            }
        }
        
        for (size_t i = 0; i < MaxBombs; i++)
        {
            bombSprites[i].setOpacity(player->abilityLevel < 5 ? 0 :
                player->numBombs > i ? 1 : 0.25);
        }
        
        drawDash = player->abilityLevel >= 3;
    }
    else
    {
        playerMeter.setTarget(0);
        dashMeter.setTarget(0);
    }
    
    playerMeter.update(curTime);
    dashMeter.update(curTime);
}

void GUI::render(Renderer& renderer)
{
    renderer.pushDrawable(guiLeft, {}, 1000);

    renderer.pushDrawable(playerMeter, {}, 1100);
    if (drawDash)
        renderer.pushDrawable(dashMeter, {}, 1100);

    renderer.pushTransform();
    renderer.currentTransform.translate(ScreenWidth - guiRight.getTextureSize().x, 0);
    renderer.pushDrawable(guiRight, {}, 1000);
    renderer.popTransform();
    
    if (!abilitySprites.empty())
    {
        renderer.pushTransform();
        renderer.currentTransform.translate(44, 440);
        
        for (const auto& sprite : abilitySprites)
        {
            renderer.pushDrawable(sprite, {}, 1100);
            renderer.currentTransform.translate(0, -36);
        }
        
        renderer.popTransform();
    }
    
    for (size_t i = 0; i < MaxBombs; i++)
    {
        renderer.pushTransform();
        renderer.currentTransform.translate(48 + 22 * (i%2), 4 + 22 * (i/2));
        renderer.pushDrawable(bombSprites[i], {}, 1100);
        renderer.popTransform();
    }
}
 
