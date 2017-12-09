#pragma once

#include "scene/Scene.hpp"
#include "objects/Room.hpp"
#include "drawables/Sprite.hpp"
#include "data/LevelData.hpp"
#include "objects/GUI.hpp"
#include "gameplay/LevelPersistentData.hpp"

#include <cppmunk/Space.h>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <type_traits>
#include <exception>

#define CP_DEBUG 0

#if CP_DEBUG
#include "drawables/ChipmunkDebugDrawable.hpp"
#endif

class Player;
class Renderer;
class ResourceManager;

struct RoomData;

class GameScene : public Scene
{
#if CP_DEBUG
    ChipmunkDebugDrawable debug;
#endif
    
    cp::Space gameSpace;
    Room room;
    std::shared_ptr<LevelData> levelData;
    LevelPersistentData levelPersistentData;

    ResourceManager &resourceManager;
    std::shared_ptr<RoomData> currentRoomData;
    std::vector<std::unique_ptr<GameObject>> gameObjects, objectsToAdd;
    size_t curRoomID, requestedID;
    bool objectsLoaded;

    const PlayerController* playerController;
    sf::Vector2f offsetPos;

    GUI gui;
    
    std::chrono::steady_clock::time_point curTime, transitionBeginTime, transitionEndTime;
    sf::Vector2f transitionTargetBegin, transitionTargetEnd;

public:
    GameScene(ResourceManager& manager);
    virtual ~GameScene() {}

    cp::Space& getGameSpace() { return gameSpace; }
    const cp::Space& getGameSpace() const { return gameSpace; }

    size_t getCurrentRoomID() { return curRoomID; }
    Room& getCurrentRoom() { return room; }
    const Room& getCurrentRoom() const { return room; }
    
    ResourceManager& getResourceManager() const { return resourceManager; }
    LevelPersistentData& getLevelPersistentData() { return levelPersistentData; }

    void loadLevel(std::string levelName);
    void loadRoom(size_t id, bool transition = false, cpVect displacement = cpVect{0,0});
    void loadRoomObjects();
    void requestRoomLoad(size_t id) { requestedID = id; }

    void addObject(std::unique_ptr<GameObject> obj);
    GameObject* getObjectByName(std::string str);

    void setPlayerController(const PlayerController& controller) { playerController = &controller; }
    const PlayerController& getPlayerController() const { return *playerController; }

    template <typename T>
    std::enable_if_t<std::is_base_of<GameObject, T>::value, T*>
    getObjectByName(std::string str) { return dynamic_cast<T*>(getObjectByName(str)); }

    std::vector<GameObject*> getObjectsByName(std::string str);
    void removeObjectsByName(std::string str);

    cpVect wrapPosition(cpVect pos);
    sf::Vector2f fitIntoRoom(sf::Vector2f vec);

    virtual void update(std::chrono::steady_clock::time_point curTime) override;
    void checkWarps();
    void checkWarp(Player* player, WarpData::Dir direction, cpVect pos);
    
    virtual void render(Renderer& renderer) override;
};

