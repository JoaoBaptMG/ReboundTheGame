#pragma once

#include "Scene.hpp"

#include "drawables/Sprite.hpp"
#include "drawables/GUIMap.hpp"

#include "input/CommonActions.hpp"
#include "ui/UIButton.hpp"
#include "ui/UIButtonGroup.hpp"
#include "ui/UIPointer.hpp"

#include "data/LevelData.hpp"

struct Settings;
class ResourceManager;
class InputManager;
class LocalizationManager;

class PauseFrame
{
public:
    PauseFrame() {}
    virtual ~PauseFrame() {}
    
    virtual void update(std::chrono::steady_clock::time_point curTime) = 0;
    virtual void render(Renderer &renderer) = 0;
    
    virtual void activate() = 0;
    virtual void deactivate() = 0;
};

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
    
    ButtonAction quitPause, switchFrameLeft, switchFrameRight;
    UIButton frameButtons[3];
    //UIButtonGroup frameButtonGroup;
    UIPointer pointer;
    
    size_t currentFrame;
    std::unique_ptr<PauseFrame> pauseFrames[3];
    
    std::shared_ptr<LevelData> mapLevelData;
    size_t mapCurrentRoom;
    sf::Vector2f mapDisplayPosition;
    const std::vector<bool>* mapVisibleMaps;

public:
    PauseScene(Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager& lm);
    virtual ~PauseScene() {}
    
    virtual void update(std::chrono::steady_clock::time_point curTime) override;
    virtual void render(Renderer &renderer) override;
    
    void setMapLevelData(std::shared_ptr<LevelData> level, size_t curRoom, sf::Vector2f pos,
        const std::vector<bool>& visibleMaps);
    void unpause();
    
    void switchPauseFrame(size_t frame);
};

