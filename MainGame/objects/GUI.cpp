#include "GUI.hpp"

#include <SFML/Graphics.hpp>

#include "scene/GameScene.hpp"
#include "resources/ResourceManager.hpp"
#include "rendering/Renderer.hpp"
#include "objects/Player.hpp"
#include "defaults.hpp"
#include "language/convenienceConfigText.hpp"
#include "resources/FontHandler.hpp"

GUI::GUI(GameScene& scene) : gameScene(scene),
    guiLeft(scene.getResourceManager().load<sf::Texture>("gui-left.png"), sf::Vector2f(0, 0)),
    guiRight(scene.getResourceManager().load<sf::Texture>("gui-right.png"), sf::Vector2f(0, 0)),
    levelLabel(scene.getResourceManager().load<FontHandler>(scene.getLocalizationManager().getFontName())),
    levelID(scene.getResourceManager().load<FontHandler>(scene.getLocalizationManager().getFontName())),
    playerMeter(MeterSize::Normal), dashMeter(MeterSize::Small, false), lastIconName(""), drawDash(false)
{
    playerMeter.setColors(sf::Color::Green, sf::Color::Red, sf::Color(80, 80, 80, 255));
    playerMeter.setPosition(4, 484);
    
    dashMeter.setColors(sf::Color(162, 0, 255, 255), sf::Color::Yellow, sf::Color(80, 80, 80, 255));
    dashMeter.setPosition(52, 484);
    dashMeter.setIcon(scene.getResourceManager().load<sf::Texture>("icon-dash.png"));
    
    for (auto& sprite : bombSprites)
        sprite.setTexture(scene.getResourceManager().load<sf::Texture>("icon-bomb.png"));
        
    configureText();
}

void GUI::configureText()
{
    levelLabel.setString(gameScene.getLocalizationManager().getString("ingame-gui-level-label"));
    levelLabel.setFontSize(
        std::strtoull(gameScene.getLocalizationManager().getString("ingame-gui-level-label-size").data(), nullptr, 0));
    levelLabel.setDefaultColor(sf::Color::White);
    //levelLabel.setWordAlignment(TextDrawable::Alignment::Center);
    levelLabel.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    levelLabel.setVerticalAnchor(TextDrawable::VertAnchor::Bottom);
    //levelLabel.setWordWrappingWidth(68);
    configTextDrawable(levelLabel, gameScene.getLocalizationManager());
    levelLabel.buildGeometry();
    
    auto builtID = gameScene.getLocalizationManager().getFormattedString("ingame-gui-level-number", {}, { { "n", 10 } });
    levelID.setString(builtID);
    levelID.setFontSize(
        std::strtoull(gameScene.getLocalizationManager().getString("ingame-gui-level-number-size").data(), nullptr, 0));
    levelID.setDefaultColor(sf::Color::White);
    //levelLabel.setWordAlignment(TextDrawable::Alignment::Center);
    levelID.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    levelID.setVerticalAnchor(TextDrawable::VertAnchor::Bottom);
    //levelID.setWordWrappingWidth(68);
    configTextDrawable(levelID, gameScene.getLocalizationManager());
    levelID.buildGeometry();
}

void GUI::update(std::chrono::steady_clock::time_point curTime)
{
    auto player = gameScene.getObjectByName<Player>("player");
    
    if (player)
    {
        playerMeter.setHeight(player->getMaxHealth());
        playerMeter.setTarget(player->getHealth());
        
        dashMeter.setHeight(player->getMaxHealth() - 62);
        dashMeter.setTarget(dashMeter.getHeight() * player->getDashDisplay());
        
        auto iconName = player->isHardballEnabled() ? "icon-player-hard.png" :
            player->isEnhanced() ? "icon-player-enhanced.png" : "icon-player.png";
            
        if (iconName != lastIconName)
        {
            playerMeter.setIcon(gameScene.getResourceManager().load<sf::Texture>(iconName));
            lastIconName = iconName;
        }
        
        /*auto prevSize = abilitySprites.size();
        abilitySprites.resize(player->abilityLevel);
        if (abilitySprites.size() > prevSize)
        {
            for (size_t i = prevSize; i < abilitySprites.size(); i++)
            {
                std::string name = "powerup" + std::to_string(i+1) + ".png";
                abilitySprites[i].setTexture(gameScene.getResourceManager().load<sf::Texture>(name));
                abilitySprites[i].setTextureRect(sf::FloatRect(4, 4, 40, 40));
            }
        }*/
        
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
    
    /*if (!abilitySprites.empty())
    {
        renderer.pushTransform();
        renderer.currentTransform.translate(44, 440);
        
        for (const auto& sprite : abilitySprites)
        {
            renderer.pushDrawable(sprite, {}, 1100);
            renderer.currentTransform.translate(0, -36);
        }
        
        renderer.popTransform();
    }*/
    
    renderer.pushTransform();
    renderer.currentTransform.translate(52, 484 - dashMeter.getFrameHeight() - 22);
    
    for (size_t i = 0; i < MaxBombs; i++)
    {
        renderer.pushDrawable(bombSprites[i], {}, 1100);
        renderer.currentTransform.translate(0, -22);
    }
    
    renderer.popTransform();
    
    renderer.pushTransform();
    renderer.currentTransform.translate(46, 516);
    renderer.pushDrawable(levelLabel, {}, 1100);
    renderer.popTransform();
    
    renderer.pushTransform();
    renderer.currentTransform.translate(46, 564);
    renderer.pushDrawable(levelID, {}, 1100);
    renderer.popTransform();
}
 
