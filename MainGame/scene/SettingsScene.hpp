#pragma once

#include "scene/Scene.hpp"

#include "drawables/Sprite.hpp"
#include "drawables/TextDrawable.hpp"

#include "input/CommonActions.hpp"
#include "ui/UIButton.hpp"
#include "ui/UIButtonGroup.hpp"
#include "ui/UIPointer.hpp"
#include "ui/UITextSwitch.hpp"
#include "ui/UISlider.hpp"

#include "language/LangID.hpp"
#include "data/LevelData.hpp"

struct Settings;
class ResourceManager;
class InputManager;
class LocalizationManager;

class SettingsBase
{
    sf::Vector2f centerPosition;
    TextDrawable titles[3];
    size_t musicVolume, soundVolume;
    bool fullscreen, vsync;
    
    UITextSwitch fullscreenSwitch, vsyncSwitch;
    UIButton languageButton;
    UISlider musicSlider, soundSlider;
    UIButton keyboardButton, joystickButton, backButton;
    UIButtonGroup buttonGroup;
    
protected:
    SettingsBase(Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager &lm,
        sf::Vector2f centerPos, UIPointer& pointer, LangID backId);
    ~SettingsBase() {}
    
    void update(std::chrono::steady_clock::time_point curTime);
    void render(Renderer& renderer);
    
    void activate();
    void deactivate();
    
    std::function<void()> backAction;
};

class SettingsScene : public Scene, public SettingsBase
{
    Sprite sceneFrame;
    UIPointer pointer;
    
public:
    SettingsScene(Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager &lm);
    virtual ~SettingsScene() {}
    
    virtual void update(std::chrono::steady_clock::time_point curTime) override { SettingsBase::update(curTime); }
    virtual void render(Renderer& renderer) override;
};
