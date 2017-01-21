#include "GameScene.hpp"
#include <functional>
#include "../objects/Player.hpp"
#include <SFML/System.hpp>

template <typename T>
T clamp(T cur, T min, T max)
{
    return cur < min ? min : cur > max ? max : cur;
}

GameScene::GameScene(ResourceManager &manager) : room(*this, manager)
{
    guiLeft.setTexture(*manager.load<sf::Texture>("gui-left.png"));
    guiRight.setTexture(*manager.load<sf::Texture>("gui-right.png"));
    guiRight.setPosition((ScreenWidth+PlayfieldWidth)/2, 0);
}

void GameScene::update(float dt)
{
    collisionHandler.update(dt);

    if (currentPlayer)
        currentPlayer->update(dt);
    room.update(dt);
}

void GameScene::render(Renderer& renderer)
{
    auto vec = currentPlayer->getDisplayPosition();
    vec.x = clamp<float>(vec.x, PlayfieldWidth/2, DefaultTileSize * room.getWidth() - PlayfieldWidth/2);
    vec.y = clamp<float>(vec.y, PlayfieldHeight/2, DefaultTileSize * room.getHeight() - PlayfieldHeight/2);

    renderer.pushTransform();
    renderer.currentTransform.translate(sf::Vector2f{ScreenWidth, ScreenHeight}/2.0f - vec);
    
    if (currentPlayer)
        currentPlayer->render(renderer);
    room.render(renderer);

    renderer.popTransform();

    renderer.pushDrawable(guiLeft, {}, 100);
    renderer.pushDrawable(guiRight, {}, 100);
}
