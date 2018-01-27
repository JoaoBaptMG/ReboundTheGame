#include "GUI.hpp"

#include <SFML/Graphics.hpp>
#include <string>
#include <cstdlib>

#include "scene/GameScene.hpp"
#include "resources/ResourceManager.hpp"
#include "rendering/Renderer.hpp"
#include "objects/Player.hpp"
#include "defaults.hpp"
#include "language/convenienceConfigText.hpp"
#include "resources/FontHandler.hpp"

#include "ColorList.hpp"

#include <chronoUtils.hpp>

constexpr float FirstBlinkPeriod = 2.0;
constexpr float SecondBlinkPeriod = 0.4;

constexpr size_t FirstBlinkThreshold = 100;
constexpr size_t SecondBlinkThreshold = 40;

struct ParsedConfigString
{
    size_t labelSize, idSize;
};

ParsedConfigString parseConfigString(std::string config)
{
    auto pos = config.find_first_of(' ');
    if (pos == std::string::npos) return { 0, 0 };
    auto pos2 = config.find_first_not_of(' ', pos);
    if (pos2 == std::string::npos) return { 0, 0 };
    
    size_t labelSize = strtoull(config.data(), nullptr, 10);
    size_t idSize = strtoull(config.data() + pos2, nullptr, 10);
    
    return { labelSize, idSize };
}

GUIBossUpdater* nullBossMeter()
{
    struct NullBossUpdater : public GUIBossUpdater
    {
        virtual size_t getCurrentHealth() const override { return 0; }
        virtual size_t getMaxHealth() const override { return 1; }
    };
    
    static NullBossUpdater nullBossUpdater;
    return &nullBossUpdater;
}

GUI::GUI(GameScene& scene) : gameScene(scene),
    guiLeft(scene.getResourceManager().load<sf::Texture>("gui-left.png"), sf::Vector2f(0, 0)),
    guiRight(scene.getResourceManager().load<sf::Texture>("gui-right.png"), sf::Vector2f(0, 0)),
    playerMeter(MeterSize::Normal), dashMeter(MeterSize::Small, false), bossMeter(MeterSize::Normal),
    levelLabel(scene.getResourceManager().load<FontHandler>(scene.getLocalizationManager().getFontName())),
    levelID(scene.getResourceManager().load<FontHandler>(scene.getLocalizationManager().getFontName())),
    currentBoss(nullptr), lastIconName(""), drawDash(false), levelNumber(1), healthBlinkPhase(0)
{
    playerMeter.setColors(sf::Color::Green, sf::Color::Red, sf::Color(80, 80, 80, 255));
    playerMeter.setPosition(4, 484);
    
    dashMeter.setColors(sf::Color(162, 0, 255, 255), sf::Color::Yellow, sf::Color(80, 80, 80, 255));
    dashMeter.setPosition(52, 484);
    dashMeter.setIcon(scene.getResourceManager().load<sf::Texture>("icon-dash.png"));
    
    bossMeter.setColors(Colors::Orange, sf::Color::Yellow, sf::Color(80, 80, 80, 255));
    bossMeter.setPosition(8, 484);
    bossMeter.setHeight(432);
    
    for (auto& sprite : bombSprites)
        sprite.setTexture(scene.getResourceManager().load<sf::Texture>("icon-bomb.png"));
        
    configureText();
}

void GUI::configureText()
{
    auto& locManager = gameScene.getLocalizationManager();
    auto config = parseConfigString(locManager.getString("ingame-gui-level-config"));
    
    levelLabel.setString(locManager.getString("ingame-gui-level-label"));
    levelLabel.setFontSize(config.labelSize);
    levelLabel.setDefaultColor(sf::Color::White);
    //levelLabel.setWordAlignment(TextDrawable::Alignment::Center);
    levelLabel.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    levelLabel.setVerticalAnchor(TextDrawable::VertAnchor::Baseline);
    //levelLabel.setWordWrappingWidth(68);
    configTextDrawable(levelLabel, gameScene.getLocalizationManager());
    levelLabel.buildGeometry();
    
    auto builtID = locManager.getFormattedString("ingame-gui-level-number", {}, { { "n", levelNumber } });
    levelID.setString(builtID);
    levelID.setFontSize(config.idSize);
    levelID.setDefaultColor(sf::Color::White);
    //levelLabel.setWordAlignment(TextDrawable::Alignment::Center);
    levelID.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    levelID.setVerticalAnchor(TextDrawable::VertAnchor::Baseline);
    //levelID.setWordWrappingWidth(68);
    configTextDrawable(levelID, gameScene.getLocalizationManager());
    levelID.buildGeometry();
}

void GUI::setLevelNumber(size_t number)
{
    levelNumber = number;
    guiMap.setCurLevel(gameScene.getCurrentLevel());
    
    guiMap.setCurRoom(gameScene.getCurrentRoomID());
    guiMap.presentRoom(gameScene.getCurrentRoomID());
    
    auto& locManager = gameScene.getLocalizationManager();
    auto builtID = locManager.getFormattedString("ingame-gui-level-number", {}, { { "n", levelNumber } });
    levelID.setString(builtID);
    levelID.buildGeometry();
}

void GUI::setVisibleMaps(const std::vector<bool>& visibleMaps)
{
    for (size_t i = 0; i < visibleMaps.size(); i++)
        if (visibleMaps.at(i)) guiMap.presentRoomFull(i);
        else guiMap.hideRoom(i);
}

void GUI::update(std::chrono::steady_clock::time_point curTime)
{
    if (lastTime == decltype(lastTime)()) lastTime = curTime;
    
    auto player = gameScene.getObjectByName<Player>("player");
    
    if (player)
    {
        auto height = playerMeter.getHeight();
        if (height == 0) playerMeter.setHeight(player->getMaxHealth());
        else if (player->getMaxHealth() > height)
            playerMeter.setHeight(height+1);
        else if (player->getMaxHealth() < height)
            playerMeter.setHeight(height-1);
        
        playerMeter.setTarget(player->getHealth());
        
        dashMeter.setHeight(playerMeter.getHeight() - 62);
        dashMeter.setTarget(dashMeter.getHeight() * player->getDashDisplay());
        
        auto iconName = player->isHardballEnabled() ? "icon-player-hard.png" :
            player->isEnhanced() ? "icon-player-enhanced.png" : "icon-player.png";
            
        if (iconName != lastIconName)
        {
            playerMeter.setIcon(gameScene.getResourceManager().load<sf::Texture>(iconName));
            lastIconName = iconName;
        }
        
        for (size_t i = 0; i < MaxBombs; i++)
        {
            bombSprites[i].setOpacity(player->abilityLevel < 5 ? 0 :
                player->numBombs > i ? 1 : 0.25);
        }
        
        drawDash = player->abilityLevel >= 3;
        auto pos = player->getDisplayPosition();
        sf::Vector2f mapPosition(std::floor(pos.x / DefaultTileSize), std::floor(pos.y / DefaultTileSize));
        guiMap.setDisplayPosition(mapPosition);
        
        size_t health = player->getHealth();
        if (healthBlinkPhase > 0 || health <= FirstBlinkThreshold)
        {
            float period = health >= FirstBlinkThreshold ? FirstBlinkPeriod :
                health <= SecondBlinkThreshold ? SecondBlinkPeriod :
                SecondBlinkPeriod + (FirstBlinkPeriod - SecondBlinkPeriod) *
                (float)(health - SecondBlinkThreshold) / (FirstBlinkThreshold - SecondBlinkThreshold);
            
            healthBlinkPhase += toSeconds<float>(curTime - lastTime) / period;
            
            while (healthBlinkPhase > 1)
            {
                if (health > FirstBlinkThreshold) healthBlinkPhase = 0;
                else healthBlinkPhase -= 1;
            }
        }
        
        sf::Color healthColor(0, 255, 0);
        healthColor.r = 255 * (0.5 - 0.5 * cosf(2 * M_PI * healthBlinkPhase));
        playerMeter.setFillColor(healthColor);
    }
    else
    {
        playerMeter.setTarget(0);
        dashMeter.setTarget(0);
    }
    
    if (currentBoss)
    {
        auto size = bossMeter.getHeight() * currentBoss->getCurrentHealth() / currentBoss->getMaxHealth();
        bossMeter.setTarget(size);
    }
    
    guiMap.setCurRoom(gameScene.getCurrentRoomID());
    guiMap.presentRoom(gameScene.getCurrentRoomID());
    
    playerMeter.update(curTime);
    dashMeter.update(curTime);
    bossMeter.update(curTime);
    guiMap.update(curTime);
    
    lastTime = curTime;
}

void GUI::render(Renderer& renderer)
{
    renderer.pushDrawable(guiLeft, {}, 6000);

    renderer.pushDrawable(playerMeter, {}, 6600);
    if (drawDash) renderer.pushDrawable(dashMeter, {}, 6600);
    
    renderer.pushTransform();
    renderer.currentTransform.translate(52, 484 - dashMeter.getFrameHeight() - 22);
    
    for (size_t i = 0; i < MaxBombs; i++)
    {
        renderer.pushDrawable(bombSprites[i], {}, 6600);
        renderer.currentTransform.translate(0, -22);
    }
    
    renderer.popTransform();
    
    renderer.pushTransform();
    renderer.currentTransform.translate(46, 516);
    renderer.pushDrawable(levelLabel, {}, 6600);
    renderer.popTransform();
    
    renderer.pushTransform();
    renderer.currentTransform.translate(46, 564);
    renderer.pushDrawable(levelID, {}, 6600);
    renderer.popTransform();
    
    renderer.pushTransform();
    renderer.currentTransform.translate(ScreenWidth - guiRight.getTextureSize().x, 0);
    renderer.pushDrawable(guiRight, {}, 6000);
    
    if (currentBoss) renderer.pushDrawable(bossMeter, {}, 6600);
    
    renderer.pushTransform();
    renderer.currentTransform.translate(50, 530);
    renderer.pushDrawable(guiMap, {}, 6600);
    renderer.popTransform();
    
    renderer.popTransform();
}
