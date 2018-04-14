#pragma once

#include "SettingsPanel.hpp"

#include "drawables/TextDrawable.hpp"
#include "ui/UIButton.hpp"
#include "ui/UIButtonGroup.hpp"
#include "ui/UIPointer.hpp"
#include "ui/UIInputRemapper.hpp"

#include "language/LangID.hpp"

struct Settings;
class ResourceManager;
class InputManager;
class LocalizationManager;

class InputMappingSettingsPanel final : public SettingsPanel
{
    TextDrawable title;
    std::vector<std::unique_ptr<UIInputRemapper>> mappingButtons;
    UIButton backButton;
    UIInputRemappingButtonGroup buttonGroup;

    void buildInputScreen(sf::Vector2f pos, InputManager& inputManager, InputSettings& settings,
        ResourceManager &rm, LocalizationManager& lm, bool forJoystick);

public:
    InputMappingSettingsPanel(Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager &lm,
        SettingsBase* curSettings, UIPointer& pointer, bool forJoystick);
    virtual ~InputMappingSettingsPanel() = default;

    virtual void update(FrameTime curTime) override;
    virtual void render(Renderer& renderer) override;

    virtual void activate() override;
    virtual void deactivate() override;
};
