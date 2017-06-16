#include "GameScene.hpp"
#include "objects/Player.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "utility/chronoUtils.hpp"
#include "data/RoomData.hpp"

#include <functional>
#include <iterator>
#include <SFML/System.hpp>

template <typename T>
T clamp(T cur, T min, T max)
{
    return cur < min ? min : cur > max ? max : cur;
}

GameScene::GameScene(ResourceManager &manager) : room(*this), resourceManager(manager), playerController(nullptr),
    gui(*this), objectsLoaded(false)
#if CP_DEBUG
, debug(gameSpace)
#endif
{
    gameSpace.setGravity(cpVect{0.0f, 1024.0f});
}

void GameScene::loadLevel(std::string levelName)
{
    levelData = resourceManager.load<LevelData>(levelName);
    loadRoom(levelData->startingRoom);
    loadRoomObjects();
}

void GameScene::loadRoom(size_t id)
{
    auto roomName = levelData->roomResourceNames.at(id) + ".map";
    
    gameObjects.erase(std::remove_if(gameObjects.begin(), gameObjects.end(),
        [](const auto& obj) { return !obj->isPersistent; }), gameObjects.end());
    
    currentRoomData = resourceManager.load<RoomData>(roomName);
    room.loadRoom(*currentRoomData);
    objectsLoaded = false;
}

void GameScene::loadRoomObjects()
{
    if (objectsLoaded) return;
    
    for (const auto& descriptor : currentRoomData->gameObjectDescriptors)
    {
        auto obj = createObjectFromDescriptor(*this, descriptor);
        if (obj) gameObjects.push_back(std::move(obj));
    }

    objectsLoaded = true;
}

void GameScene::addObject(std::unique_ptr<GameObject> obj)
{
    if (!obj) return;
    objectsToAdd.push_back(std::move(obj));
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

void GameScene::removeObjectsByName(std::string str)
{
    for (auto obj : getObjectsByName(str)) obj->remove();
}

cpVect GameScene::wrapPosition(cpVect pos)
{
    cpFloat width = DefaultTileSize * currentRoomData->mainLayer.width();
    cpFloat height = DefaultTileSize * currentRoomData->mainLayer.height();
    
    pos.x -= width * std::floor(pos.x/width);
    pos.y -= height * std::floor(pos.y/height);

    return pos;
}

void GameScene::update(std::chrono::steady_clock::time_point curTime)
{
    using std::swap;
    
    gameSpace.step(toSeconds<cpFloat>(UpdateFrequency));

    room.update(curTime);

    for (const auto& obj : gameObjects) obj->update(curTime);

    gameObjects.erase(std::remove_if(gameObjects.begin(), gameObjects.end(),
        [](const auto& obj) { return obj->shouldRemove; }), gameObjects.end());

    std::move(objectsToAdd.begin(), objectsToAdd.end(), std::back_inserter(gameObjects));
    objectsToAdd.clear();

    gui.update(curTime);

    checkWarps();
}

void GameScene::checkWarps()
{
    auto player = getObjectByName<Player>("player");

    if (player)
    {
        auto pos = player->getPosition();

        if (pos.x >= DefaultTileSize * currentRoomData->mainLayer.width())
        {
            pos.x -= DefaultTileSize * currentRoomData->mainLayer.width();
            checkWarp(player, WarpData::Dir::Right, pos);
            player->setPosition(pos);
        }
        else if (pos.y >= DefaultTileSize * currentRoomData->mainLayer.height())
        {
            pos.y -= DefaultTileSize * currentRoomData->mainLayer.height();
            checkWarp(player, WarpData::Dir::Down, pos);
            player->setPosition(pos);
        }
        else if (pos.x < 0)
        {
            checkWarp(player, WarpData::Dir::Left, pos);
            pos.x += DefaultTileSize * currentRoomData->mainLayer.width();
            player->setPosition(pos);
        }
        else if (pos.y < 0)
        {
            checkWarp(player, WarpData::Dir::Up, pos);
            pos.y += DefaultTileSize * currentRoomData->mainLayer.height();
            player->setPosition(pos);
        }

        if (!objectsLoaded) loadRoomObjects();
    }
}

void GameScene::checkWarp(Player* player, WarpData::Dir direction, cpVect &pos)
{
    bool isHorizontal = direction == WarpData::Dir::Right || direction == WarpData::Dir::Left;
    auto c = isHorizontal ? pos.y : pos.x;

    WarpData warp = { 0, 0, (uint16_t)-1, 16383, WarpData::Dir::Up };
    for (const auto& curWarp : currentRoomData->warps)
    {
        if (curWarp.warpDir != direction) continue;
        if (c < curWarp.c1 || c >= curWarp.c2) continue;
        warp = curWarp;
    }

    if (warp.roomId != (uint16_t)-1)
    {
        loadRoom(warp.roomId);
        auto delta = currentRoomData->warps[warp.warpId].c1 - warp.c1;
        if (isHorizontal) pos.y += delta;
        else pos.x += delta;
    }
}

void GameScene::render(Renderer& renderer)
{
    renderer.pushTransform();

    auto player = getObjectByName<Player>("player");

    if (player)
    {
        auto vec = player->getDisplayPosition();
        vec.x = clamp<float>(vec.x, PlayfieldWidth/2,
            DefaultTileSize * currentRoomData->mainLayer.width() - PlayfieldWidth/2);
        vec.y = clamp<float>(vec.y, PlayfieldHeight/2,
            DefaultTileSize * currentRoomData->mainLayer.height() - PlayfieldHeight/2);

        offsetPos = vec;
    }

    gui.render(renderer);
    renderer.currentTransform.translate(sf::Vector2f{(float)ScreenWidth, (float)ScreenHeight}/2.0f - offsetPos);
    
    room.render(renderer);
    for (const auto& obj : gameObjects) obj->render(renderer);

#if CP_DEBUG
    renderer.pushDrawable(debug, {}, 800);
#endif

    renderer.popTransform();
}
