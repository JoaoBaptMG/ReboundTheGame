#include "GameScene.hpp"
#include "objects/Player.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "defaults.hpp"

#include <functional>
#include <SFML/System.hpp>

template <typename T>
T clamp(T cur, T min, T max)
{
    return cur < min ? min : cur > max ? max : cur;
}

GameScene::GameScene(ResourceManager &manager) : room(*this), resourceManager(manager), playerController(nullptr)
#if CP_DEBUG
, debug(gameSpace)
#endif
{
    gameSpace.setGravity({ 0.0f, 1024.0f });
    
    guiLeft.setTexture(*manager.load<sf::Texture>("gui-left.png"));
    guiRight.setTexture(*manager.load<sf::Texture>("gui-right.png"));
    guiRight.setPosition((ScreenWidth+PlayfieldWidth)/2, 0);
}

void GameScene::addObject(std::unique_ptr<GameObject> obj)
{
    if (!obj) return;
    gameObjects.push_back(std::move(obj));
}

GameObject* GameScene::getObjectByName(std::string str)
{
    for (const auto& obj : gameObjects)
        if (obj->getName() == str)
            return obj.get();

    return nullptr;
}

std::vector<GameObject*> GameScene::getObjectsByName(std::string str)
{
    std::vector<GameObject*> objs;

    for (const auto& obj : gameObjects)
        if (obj->getName() == str)
            objs.push_back(obj.get());

    return objs;
}

void GameScene::update(std::chrono::steady_clock::time_point curTime)
{
    using FloatSeconds = std::chrono::duration<cpFloat>;
    gameSpace.step(std::chrono::duration_cast<FloatSeconds>(UpdateFrequency).count());

    room.update(curTime);

    for (const auto& obj : gameObjects) obj->update(curTime);

    gameObjects.erase(std::remove_if(gameObjects.begin(), gameObjects.end(),
        [](const auto& obj) { return obj->shouldRemove; }), gameObjects.end());
}

void GameScene::render(Renderer& renderer)
{
    renderer.pushTransform();

    auto player = getObjectByName<Player>("player");

    if (player)
    {
        auto vec = player->getDisplayPosition();
        vec.x = clamp<float>(vec.x, PlayfieldWidth/2, DefaultTileSize * room.getWidth() - PlayfieldWidth/2);
        vec.y = clamp<float>(vec.y, PlayfieldHeight/2, DefaultTileSize * room.getHeight() - PlayfieldHeight/2);

        renderer.currentTransform.translate(sf::Vector2f{ScreenWidth, ScreenHeight}/2.0f - vec);
    }
    
    room.render(renderer);
    for (const auto& obj : gameObjects) obj->render(renderer);

#if CP_DEBUG
    renderer.pushDrawable(debug, {}, 20);
#endif

    renderer.popTransform();

    renderer.pushDrawable(guiLeft, {}, 100);
    renderer.pushDrawable(guiRight, {}, 100);
}
