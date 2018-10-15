//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#pragma once

#include "scene/Scene.hpp"
#include "objects/Room.hpp"
#include "drawables/Sprite.hpp"
#include "data/LevelData.hpp"
#include "objects/GUI.hpp"
#include "objects/Camera.hpp"
#include "objects/LevelTransition.hpp"
#include "objects/MessageBox.hpp"
#include "gameplay/LevelPersistentData.hpp"

#include "settings/Settings.hpp"
#include "gameplay/SavedGame.hpp"
#include "gameplay/Script.hpp"
#include "input/InputPlayerController.hpp"
#include "input/CommonActions.hpp"
#include "language/LocalizationManager.hpp"
#include "audio/AudioManager.hpp"
#include "Services.hpp"

#include <cppmunk/Space.h>

#include <chronoUtils.hpp>
#include <type_traits>
#include <exception>

#define CP_DEBUG 0

#if CP_DEBUG
#include "drawables/ChipmunkDebugDrawable.hpp"
#endif

class Player;
class Renderer;

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

    std::shared_ptr<RoomData> currentRoomData;
    std::vector<std::unique_ptr<GameObject>> gameObjects, objectsToAdd;
    size_t curRoomID, requestedID;
    bool objectsLoaded, pausing;
    std::vector<bool> visibleMaps;

    Services& services;
    SavedGame savedGame;

    InputPlayerController inputPlayerController;
    const PlayerController* currentPlayerController;
    glm::vec2 offsetPos;

    GUI gui;
    Camera camera;
    LevelTransition levelTransition;
    MessageBox messageBox;
    Script cutsceneScript;
    StringSpecifierMap keysMap, joystickMap;
    
    FrameTime curTime;
    FrameDuration pauseLag;

public:
    GameScene(Services& services, SavedGame sg);
	virtual ~GameScene();

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
    
    MessageBox& getMessageBox() { return messageBox; }
    Camera& getCamera() { return camera; }
    
    ResourceManager& getResourceManager() const { return services.resourceManager; }
    LocalizationManager& getLocalizationManager() const { return services.localizationManager; }
    AudioManager& getAudioManager() const { return services.audioManager; }
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

    const StringSpecifierMap& getInputSpecifierMap() const;

    const PlayerController& getPlayerController() const
    {
        return currentPlayerController ? *currentPlayerController : inputPlayerController;
    }
    
    void setPlayerController(const PlayerController& controller);
    void resetPlayerController();

    void addObject(std::unique_ptr<GameObject> obj);
    GameObject* getObjectByName(std::string str);

    template <typename T>
    std::enable_if_t<std::is_base_of<GameObject, T>::value && !std::is_same<GameObject, T>::value, T*>
    getObjectByName(std::string str) { return dynamic_cast<T*>(getObjectByName(str)); }

    std::vector<GameObject*> getObjectsByName(std::string str);
    void removeObjectsByName(std::string str);

    Script& getCutsceneScript() { return cutsceneScript; }
    const Script& getCutsceneScript() const { return cutsceneScript; }

    void runCutsceneScript(Script::ScriptFunction function);
    void playSound(std::string soundName);

    cpVect wrapPosition(cpVect pos);
    glm::vec2 fitIntoRoom(glm::vec2 vec);

    virtual void update(FrameTime curTime) override;
    void checkWarps();
    void checkWarp(Player* player, WarpData::Dir direction, cpVect pos);
    void notifyTransitionEnded();
    
    void setCurrentBoss(GUIBossUpdater* boss) { gui.setCurrentBoss(boss); }
    
    virtual void render(Renderer& renderer) override;
    
    virtual void pause() override;
    virtual void resume() override;
};

