#pragma once

#include "SettingsPanel.hpp"

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

class RootSettingsPanel final : public SettingsPanel
{
    size_t musicVolume, soundVolume;
    bool fullscreen, vsync;

    TextDrawable titles[3];
    UITextSwitch fullscreenSwitch, vsyncSwitch;
    UIButton languageButton;
    UISlider musicSlider, soundSlider;
    UIButton keyboardButton, joystickButton, backButton;
    UIButtonGroup buttonGroup;

public:
    RootSettingsPanel(Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager &lm,
        SettingsBase* curSettings, UIPointer& pointer);
    virtual ~RootSettingsPanel() = default;

    virtual void update(std::chrono::steady_clock::time_point curTime) override;
    virtual void render(Renderer& renderer) override;

    virtual void activate() override;
    virtual void deactivate() override;
};