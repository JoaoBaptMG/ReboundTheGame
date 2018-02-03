#pragma once

#include "scene/Scene.hpp"
#include "objects/Room.hpp"
#include "drawables/Sprite.hpp"
#include "data/LevelData.hpp"
#include "objects/GUI.hpp"
#include "objects/Camera.hpp"
#include "objects/LevelTransition.hpp"
#include "gameplay/LevelPersistentData.hpp"

#include "settings/Settings.hpp"
#include "gameplay/SavedGame.hpp"
#include "input/PlayerController.hpp"
#include "input/CommonActions.hpp"

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
class LocalizationManager;
class InputManager;

struct RoomData;

class GameScene : public Scene
{
#if CP_DEBUG
    ChipmunkDebugDrawable debug;
#endif
    
    enum class NextScene { None, Pause, Gameover, Advance } sceneRequested;
    std::string nextLevelRequested;
    
    cp::Space gameSpace;
    Room room;
    std::shared_ptr<LevelData> levelData;
    std::string levelName;
    LevelPersistentData levelPersistentData;

    ResourceManager &resourceManager;
    std::shared_ptr<RoomData> currentRoomData;
    std::vector<std::unique_ptr<GameObject>> gameObjects, objectsToAdd;
    size_t curRoomID, requestedID;
    bool objectsLoaded, pausing;
    std::vector<bool> visibleMaps;

    LocalizationManager &localizationManager;
    InputManager& inputManager;
    Settings& settings;
    SavedGame savedGame;

    PlayerController playerController;
    sf::Vector2f offsetPos;

    GUI gui;
    Camera camera;
    LevelTransition levelTransition;
    
    std::chrono::steady_clock::time_point curTime;
    std::chrono::duration<size_t, std::milli> pauseLag;

public:
    GameScene(Settings& settings, SavedGame sg, InputManager& im, ResourceManager& rm, LocalizationManager &lm);
    virtual ~GameScene() {}

    cp::Space& getGameSpace() { return gameSpace; }
    const cp::Space& getGameSpace() const { return gameSpace; }

    size_t getCurrentRoomID() { return curRoomID; }
    Room& getCurrentRoom() { return room; }
    const Room& getCurrentRoom() const { return room; }
    
    auto getCurrentLevel() const { return levelData; }
    auto getRoomWidth() const { return currentRoomData->mainLayer.width(); }
    auto getRoomHeight() const { return currentRoomData->mainLayer.height(); }
    
    auto& getSavedGame() { return savedGame; }
    const auto& getSavedGame() const { return savedGame; }
    
    ResourceManager& getResourceManager() const { return resourceManager; }
    LocalizationManager& getLocalizationManager() const { return localizationManager; }
    LevelPersistentData& getLevelPersistentData() { return levelPersistentData; }

    void loadLevel(std::string levelName);
    void reloadLevel();
    void loadRoom(size_t id, bool transition = false, cpVect displacement = cpVect{0,0}, bool deletePersistent = false);
    void loadRoomObjects();
    
    void requestRoomLoad(size_t id) { requestedID = id; }
    void requestLevelReload() { levelTransition.requestLevelTransition(""); }
    void requestLevelLoad(std::string levelName) { levelTransition.requestLevelTransition(levelName); }
    void requestPauseScreen() { sceneRequested = NextScene::Pause; }
    void requestGameoverScene() { sceneRequested = NextScene::Gameover; }
    void requestAdvanceScene(std::string nextLevel)
    { 
        sceneRequested = NextScene::Advance;
        nextLevelRequested = nextLevel;
    }

    void addObject(std::unique_ptr<GameObject> obj);
    GameObject* getObjectByName(std::string str);

    const PlayerController& getPlayerController() const { return playerController; }

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
    void notifyTransitionEnded();
    
    void setCurrentBoss(GUIBossUpdater* boss) { gui.setCurrentBoss(boss); }
    
    virtual void render(Renderer& renderer) override;
    
    virtual void pause() override;
};

