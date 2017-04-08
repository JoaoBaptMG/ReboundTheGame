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
    playerMeter(MeterSize::Normal)
{
    playerMeter.setColors(sf::Color::Green, sf::Color::Red, sf::Color(80, 80, 80, 255));
    playerMeter.setPosition(4, 484);
}

void GUI::update(std::chrono::steady_clock::time_point curTime)
{
    auto player = gameScene.getObjectByName<Player>("player");
    playerMeter.setHeight(player->getMaxHealth());
    playerMeter.setTarget(player->getHealth());
    
    playerMeter.update(curTime);
}

void GUI::render(Renderer& renderer)
{
    renderer.pushDrawable(guiLeft, {}, 1000);

    renderer.pushDrawable(playerMeter, {}, 1100);

    renderer.pushTransform();
    renderer.currentTransform.translate(ScreenWidth - guiRight.getTextureSize().x, 0);
    renderer.pushDrawable(guiRight, {}, 1000);
    renderer.popTransform();
}
 
