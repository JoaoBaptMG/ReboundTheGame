#pragma once

#include "scene/Scene.hpp"
#include "PauseFrame.hpp"

#include "drawables/Sprite.hpp"
#include "input/CommonActions.hpp"
#include "ui/UIButton.hpp"
#include "ui/UIButtonGroup.hpp"
#include "ui/UIPointer.hpp"

#include "language/LocalizationManager.hpp"

struct Settings;
class ResourceManager;
class InputManager;
class LocalizationManager;

class LevelData;

class PauseScene final : public Scene
{
    Settings& settings;
    InputManager& inputManager;
    ResourceManager& resourceManager;
    LocalizationManager& localizationManager;
    
    Sprite backgroundSprite;
    std::chrono::steady_clock::time_point curTime, transitionTime;
    float transitionFactor;
    bool unpausing;
    
    ButtonAction<2> quitPause, switchFrameLeft, switchFrameRight;
    UIButton frameButtons[3];
    //UIButtonGroup frameButtonGroup;
    UIPointer pointer;
    
    size_t currentFrame;
    std::unique_ptr<PauseFrame> pauseFrames[3];

    LocalizationManager::CallbackEntry callbackEntry;

public:
    PauseScene(Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager& lm);
    virtual ~PauseScene() {}
    
    virtual void update(std::chrono::steady_clock::time_point curTime) override;
    virtual void render(Renderer &renderer) override;
    
    void setMapLevelData(std::shared_ptr<LevelData> level, size_t curRoom, sf::Vector2f pos,
        const std::vector<bool>& visibleMaps);
    void setCollectedFrameSavedGame(const SavedGame& sg);
    void unpause();
    
    void switchPauseFrame(size_t frame);
};

