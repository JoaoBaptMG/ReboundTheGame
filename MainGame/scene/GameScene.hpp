#pragma once

#include "scene/Scene.hpp"
#include "objects/Room.hpp"
#include "drawables/Sprite.hpp"
#include "data/LevelData.hpp"

#include <cppmunk/Space.h>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <type_traits>

#define CP_DEBUG 0

#if CP_DEBUG
#include "drawables/ChipmunkDebugDrawable.hpp"
#endif

class Player;
class Renderer;
class RoomData;
class ResourceManager;

class GameScene : public Scene
{
#if CP_DEBUG
    ChipmunkDebugDrawable debug;
#endif
    
    Chipmunk::Space gameSpace;
    Room room;
    std::shared_ptr<LevelData> levelData;

    ResourceManager &resourceManager;
    std::shared_ptr<RoomData> currentRoomData;
    std::vector<std::unique_ptr<GameObject>> gameObjects;

    const PlayerController* playerController;
    sf::Vector2f offsetPos;

public:
    GameScene(ResourceManager& manager);
    virtual ~GameScene() {}

    Chipmunk::Space& getGameSpace() { return gameSpace; }
    const Chipmunk::Space& getGameSpace() const { return gameSpace; }

    Room& getCurrentRoom() { return room; }
    const Room& getCurrentRoom() const { return room; }
    
    ResourceManager& getResourceManager() const { return resourceManager; }

    void loadLevel(std::string levelName);
    void loadRoom(size_t id);

    void addObject(std::unique_ptr<GameObject> obj);
    GameObject* getObjectByName(std::string str);

    void setPlayerController(const PlayerController& controller) { playerController = &controller; }
    const PlayerController& getPlayerController() const { return *playerController; }

    template <typename T>
    std::enable_if_t<std::is_base_of<GameObject, T>::value, T*>
    getObjectByName(std::string str) { return dynamic_cast<T*>(getObjectByName(str)); }

    std::vector<GameObject*> getObjectsByName(std::string str);

    virtual void update(std::chrono::steady_clock::time_point curTime) override;
    void checkWarps();
    void checkWarp(Player* player, WarpData::Dir direction, cpVect &pos);
    
    virtual void render(Renderer& renderer) override;
};

