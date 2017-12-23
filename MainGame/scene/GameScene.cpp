#include "GameScene.hpp"
#include "objects/Player.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include <chronoUtils.hpp>
#include <assert.hpp>
#include "data/RoomData.hpp"
#include "defaults.hpp"

#include <functional>
#include <iterator>
#include <SFML/System.hpp>

constexpr auto TransitionDuration = 20 * UpdateFrequency;

template <typename T>
T clamp(T cur, T min, T max)
{
    return cur < min ? min : cur > max ? max : cur;
}

GameScene::GameScene(ResourceManager &rm, LocalizationManager& lm) : room(*this), resourceManager(rm),
    localizationManager(lm), playerController(nullptr), gui(*this), objectsLoaded(false), curRoomID(-1),
    requestedID(-1), transitionBeginTime(), transitionEndTime()
#if CP_DEBUG
, debug(gameSpace)
#endif
{
    gameSpace.setGravity(cpVect{0.0f, 1024.0f});
}

void GameScene::loadLevel(std::string levelName)
{
    levelData = resourceManager.load<LevelData>(levelName);
    levelPersistentData.clear();
    loadRoom(levelData->startingRoom);
    loadRoomObjects();
}

void GameScene::loadRoom(size_t id, bool transition, cpVect displacement)
{
    curRoomID = id;
    auto roomName = levelData->roomResourceNames.at(id) + ".map";
    
    if (!transition)
    {
        gameObjects.erase(std::remove_if(gameObjects.begin(), gameObjects.end(),
            [](const auto& obj) { return !obj->isPersistent; }), gameObjects.end());
    }
    else
    {
        room.clearTransition();
        gameObjects.erase(std::remove_if(gameObjects.begin(), gameObjects.end(),
            [](const auto& obj) { return obj->transitionState; }), gameObjects.end());
        
        for (auto& obj : gameObjects)
        {
            bool shouldKeep = obj->notifyScreenTransition(displacement);
            if (!obj->isPersistent)
            {
                if (shouldKeep) obj->transitionState = true;
                else obj.reset(nullptr);
            }
        }
        
        gameObjects.erase(std::remove_if(gameObjects.begin(), gameObjects.end(),
            [](const auto& obj) { return !obj; }), gameObjects.end());
            
        transitionBeginTime = curTime;
        transitionEndTime = curTime + TransitionDuration;
    }
    
    currentRoomData = resourceManager.load<RoomData>(roomName);
    room.loadRoom(*currentRoomData, transition, displacement);
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
        if (obj && obj->getName() == str)
            return obj.get();

    return nullptr;
}

std::vector<GameObject*> GameScene::getObjectsByName(std::string str)
{
    std::vector<GameObject*> objs;

    for (const auto& obj : gameObjects)
        if (obj && obj->getName() == str)
            objs.push_back(obj.get());

    return objs;
}

void GameScene::removeObjectsByName(std::string str)
{
    for (auto obj : getObjectsByName(str)) if (obj) obj->remove();
}

cpVect GameScene::wrapPosition(cpVect pos)
{
    cpFloat width = DefaultTileSize * currentRoomData->mainLayer.width();
    cpFloat height = DefaultTileSize * currentRoomData->mainLayer.height();
    
    pos.x -= width * std::floor(pos.x/width);
    pos.y -= height * std::floor(pos.y/height);

    return pos;
}

sf::Vector2f GameScene::fitIntoRoom(sf::Vector2f vec)
{
    vec.x = clamp<float>(vec.x, PlayfieldWidth/2,
        DefaultTileSize * currentRoomData->mainLayer.width() - PlayfieldWidth/2);
    vec.y = clamp<float>(vec.y, PlayfieldHeight/2,
        DefaultTileSize * currentRoomData->mainLayer.height() - PlayfieldHeight/2);
    return vec;
}

void GameScene::update(std::chrono::steady_clock::time_point curTime)
{
    this->curTime = curTime;
    
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
    
    if (transitionEndTime != decltype(transitionEndTime)() && curTime > transitionEndTime)
    {
        reset(transitionBeginTime);
        reset(transitionEndTime);
        room.clearTransition();
        gameObjects.erase(std::remove_if(gameObjects.begin(), gameObjects.end(),
            [](const auto& obj) { return obj->transitionState; }), gameObjects.end());
    }
    
    if (requestedID != -1)
    {
        loadRoom(requestedID);
        loadRoomObjects();
        requestedID = -1;
    }
}

void GameScene::checkWarps()
{
    auto player = getObjectByName<Player>("player");

    if (player)
    {
        auto pos = player->getPosition();
        
        if (pos.x >= DefaultTileSize * currentRoomData->mainLayer.width())
        {
            transitionTargetBegin.x = -(float)PlayfieldWidth/2;
            transitionTargetEnd.x = (float)PlayfieldWidth/2;
            checkWarp(player, WarpData::Dir::Right, pos);
            transitionTargetBegin.y = transitionTargetEnd.y = NAN;
        }
        else if (pos.y >= DefaultTileSize * currentRoomData->mainLayer.height())
        {
            transitionTargetBegin.y = -(float)PlayfieldHeight/2;
            transitionTargetEnd.y = (float)PlayfieldHeight/2;
            checkWarp(player, WarpData::Dir::Down, pos);
            transitionTargetBegin.x = transitionTargetEnd.x = NAN;
        }
        else if (pos.x < 0)
        {
            checkWarp(player, WarpData::Dir::Left, pos);
            transitionTargetBegin.y = transitionTargetEnd.y = NAN;
            transitionTargetBegin.x = (float)DefaultTileSize * currentRoomData->mainLayer.width() + 
                (float)PlayfieldWidth/2;
            transitionTargetEnd.x = transitionTargetBegin.x - PlayfieldWidth;
        }
        else if (pos.y < 0)
        {
            checkWarp(player, WarpData::Dir::Up, pos);
            transitionTargetBegin.x = transitionTargetEnd.x = NAN;
            transitionTargetBegin.y = (float)DefaultTileSize * currentRoomData->mainLayer.height() + 
                (float)PlayfieldHeight/2;
            transitionTargetEnd.y = transitionTargetBegin.y - PlayfieldHeight;
        }
        if (!objectsLoaded) loadRoomObjects();
    }
}

void GameScene::checkWarp(Player* player, WarpData::Dir direction, cpVect pos)
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
        cpVect displacement = { 0, 0 };
        auto roomName = levelData->roomResourceNames.at(warp.roomId) + ".map";
        auto destRoomData = resourceManager.load<RoomData>(roomName);
        auto delta = destRoomData->warps[warp.warpId].c1 - warp.c1;
        
        switch (direction)
        {
            case WarpData::Dir::Right:
                displacement.x = -(cpFloat)DefaultTileSize * currentRoomData->mainLayer.width();
                displacement.y = delta; break;
            case WarpData::Dir::Down:
                displacement.y = -(cpFloat)DefaultTileSize * currentRoomData->mainLayer.height();
                displacement.x = delta; break;
            case WarpData::Dir::Left:
                displacement.x = (cpFloat)DefaultTileSize * destRoomData->mainLayer.width();
                displacement.y = delta; break;
            case WarpData::Dir::Up:
                displacement.y = (cpFloat)DefaultTileSize * destRoomData->mainLayer.height();
                displacement.x = delta; break;
        }
        
        loadRoom(warp.roomId, true, displacement);
    }
}

void GameScene::render(Renderer& renderer)
{
    renderer.pushTransform();

    auto player = getObjectByName<Player>("player");
    if (player) offsetPos = fitIntoRoom(player->getDisplayPosition());
    if (transitionEndTime != decltype(transitionEndTime)())
    {
        float dt = toSeconds<float>(curTime - transitionBeginTime) /
            toSeconds<float>(transitionEndTime - transitionBeginTime);
         
        sf::Vector2f transPos = transitionTargetBegin + dt * (transitionTargetEnd - transitionTargetBegin);
        if (!isnan(transPos.x)) offsetPos.x = transPos.x;
        if (!isnan(transPos.y)) offsetPos.y = transPos.y;
    }
    
    gui.render(renderer);
    renderer.currentTransform.translate(sf::Vector2f{(float)ScreenWidth, (float)ScreenHeight}/2.0f - offsetPos);
    
    room.render(renderer, transitionEndTime != decltype(transitionEndTime)());
    for (const auto& obj : gameObjects) obj->render(renderer);

#if CP_DEBUG
    renderer.pushDrawable(debug, {}, 800);
#endif

    renderer.popTransform();
}
