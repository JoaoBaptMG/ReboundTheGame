#pragma once

#include "scene/Scene.hpp"
#include "scene/pause/PauseFrame.hpp"
#include "SettingsPanel.hpp"

#include "drawables/Sprite.hpp"
#include "drawables/TextDrawable.hpp"

#include "input/CommonActions.hpp"
#include "ui/UIButton.hpp"
#include "ui/UIButtonGroup.hpp"
#include "ui/UIPointer.hpp"
#include "ui/UITextSwitch.hpp"
#include "ui/UISlider.hpp"

#include "language/LangID.hpp"

struct Settings;
class ResourceManager;
class InputManager;
class LocalizationManager;

class PauseScene;

class SettingsBase
{
    sf::Vector2f centerPosition;
    std::unique_ptr<SettingsPanel> curSettingsPanel;
    SettingsPanel* nextSettingsPanel;
    LangID backId;
    
protected:
    SettingsBase(Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager &lm,
        sf::Vector2f centerPos, UIPointer& pointer, LangID backId);
    ~SettingsBase() = default;
    
    void update(std::chrono::steady_clock::time_point curTime);
    void render(Renderer& renderer);
    
    void activate();
    void deactivate();

    std::function<void()> backAction;

public:
    void changeSettingsPanel(SettingsPanel* panel);

    friend class SettingsPanel;
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

class SettingsPauseFrame : public SettingsBase, public PauseFrame
{
public:
    SettingsPauseFrame(Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager& lm,
        UIPointer& pointer, PauseScene* scene);
    virtual ~SettingsPauseFrame() {}
    
    virtual void update(std::chrono::steady_clock::time_point curTime) override { SettingsBase::update(curTime); }
    virtual void render(Renderer &renderer) override;
    
    virtual void activate() override { SettingsBase::activate(); }
    virtual void deactivate() override { SettingsBase::deactivate(); }
};
