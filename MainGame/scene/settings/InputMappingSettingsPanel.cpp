#include "InputMappingSettingsPanel.hpp"

#include "SettingsScene.hpp"
#include "RootSettingsPanel.hpp"

#include "resources/ResourceManager.hpp"
#include "language/LocalizationManager.hpp"
#include "language/convenienceConfigText.hpp"
#include "rendering/Renderer.hpp"

#include "settings/Settings.hpp"
#include "input/InputSource.hpp"

#include "ui/UIButtonCommons.hpp"

#include <defaults.hpp>

constexpr float FrameWidth = PlayfieldWidth;
constexpr float ButtonHeight = 32;
constexpr float ButtonSpace = 4;
constexpr float ButtonCaptionSize = 28;
constexpr float TotalHeight = 500;

InputMappingSettingsPanel::InputMappingSettingsPanel(Settings& settings, InputManager& im, ResourceManager& rm,
    LocalizationManager &lm, SettingsBase* curSettings, UIPointer& pointer, bool forJoystick)
    : SettingsPanel(curSettings, pointer), backButton(im), buttonGroup(im, settings.inputSettings)
{
    auto pos = getCenterPosition() + sf::Vector2f(0, -TotalHeight/2 + ButtonHeight/2);

    title.setFontHandler(rm.load<FontHandler>(lm.getFontName()));
    title.setString(lm.getString(forJoystick ? "settings-joystick-title" : "settings-keyboard-title"));
    title.setFontSize(ButtonCaptionSize);
    title.setDefaultColor(sf::Color::Yellow);
    title.setOutlineThickness(1);
    title.setDefaultOutlineColor(sf::Color::Black);
    title.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    title.setVerticalAnchor(TextDrawable::VertAnchor::Center);
    configTextDrawable(title, lm);
    title.buildGeometry();

    pos += sf::Vector2f(0, ButtonHeight + ButtonSpace);
    if (forJoystick) initJoystickInputScreen(pos, im, settings.inputSettings, rm, lm);
    else initKeyboardInputScreen(pos, im, settings.inputSettings, rm, lm);

    createCommonTextualButton(backButton, rm, lm, "ui-select-field.png", "ui-select-field.png",
        sf::FloatRect(16, 0, 8, 1), sf::FloatRect(0, 0, FrameWidth - 2 * ButtonSpace, ButtonHeight),
        "settings-back-to-root", ButtonCaptionSize, sf::Color::White, 1, sf::Color::Black, sf::Vector2f(24, 0),
        TextDrawable::Alignment::Center);
    backButton.setPosition(getCenterPosition() + sf::Vector2f(0, TotalHeight/2 - ButtonHeight/2));

    backButton.getPressedSprite()->setBlendColor(sf::Color::Yellow);
    backButton.getActiveSprite()->setOpacity(0.5);
    backButton.getActiveSprite()->setOpacity(0.5);
    backButton.setDepth(3200);

    backButton.setPressAction([&, curSettings = this->curSettings]
    {
        if (forJoystick) im.addPickAllJoystickCallback(InputManager::Callback());
        else im.addPickAllKeyboardCallback(InputManager::Callback());

        curSettings->changeSettingsPanel(new RootSettingsPanel(settings, im, rm, lm, curSettings, pointer));
    });

    std::vector<UIButton*> buttons;
    for (auto& button : mappingButtons) buttons.push_back(button.get());
    buttons.push_back(&backButton);
    buttonGroup.setButtons(buttons);
    buttonGroup.setPointer(pointer);
}

void InputMappingSettingsPanel::initKeyboardInputScreen(sf::Vector2f pos, InputManager& inputManager,
    InputSettings& settings, ResourceManager &rm, LocalizationManager& lm)
{
    static const struct { LangID langID; InputSource& source; } KeyboardMappings[] =
    {
        { "input-settings-dash", settings.keyboardSettings.dashInput },
        { "input-settings-jump", settings.keyboardSettings.jumpInput },
        { "input-settings-bomb", settings.keyboardSettings.bombInput },

        { "input-settings-keyboard-left", settings.keyboardSettings.moveLeft },
        { "input-settings-keyboard-right", settings.keyboardSettings.moveRight },
        { "input-settings-keyboard-up", settings.keyboardSettings.moveUp },
        { "input-settings-keyboard-down", settings.keyboardSettings.moveDown },

        { "input-settings-switch-screen-left", settings.keyboardSettings.switchScreenLeft },
        { "input-settings-switch-screen-right", settings.keyboardSettings.switchScreenRight },
        { "input-settings-pause", settings.keyboardSettings.pauseInput },
        { "input-settings-ok", settings.keyboardSettings.okInput },
        { "input-settings-cancel", settings.keyboardSettings.cancelInput },
    };

    for (const auto& mapping : KeyboardMappings)
    {
        mappingButtons.emplace_back(std::make_unique<UIInputRemapper>(inputManager, mapping.source, lm, false));
        auto& button = mappingButtons.back();

        createCommonInputRemapper(*button, rm, lm, "ui-select-field.png", "ui-select-field.png",
            sf::FloatRect(16, 0, 8, 1), sf::FloatRect(0, 0, FrameWidth - 2 * ButtonSpace, ButtonHeight),
            mapping.langID, ButtonCaptionSize, sf::Color::White, 1, sf::Color::Black, sf::Vector2f(24, 0));
        button->setPosition(pos);

        button->getPressedSprite()->setBlendColor(sf::Color::Yellow);
        button->getActiveSprite()->setOpacity(0.5);
        button->getActiveSprite()->setOpacity(0.5);
        button->setDepth(3200);

        pos += sf::Vector2f(0, ButtonHeight + ButtonSpace);
    }

    mappingButtons.shrink_to_fit();

    buttonGroup.setGroupingFunction([](size_t i)
    {
        if (i >= 0 && i <= 2) return std::make_pair(0, 2);
        else if (i >= 3 && i <= 6) return std::make_pair(3, 6);
        else return std::make_pair(7, 11);
    });
}

void InputMappingSettingsPanel::initJoystickInputScreen(sf::Vector2f pos, InputManager& inputManager,
    InputSettings& settings, ResourceManager &rm, LocalizationManager& lm)
{
    static const struct { LangID langID; InputSource& source; } JoystickMappings[] =
    {
        { "input-settings-dash", settings.joystickSettings.dashInput },
        { "input-settings-jump", settings.joystickSettings.jumpInput },
        { "input-settings-bomb", settings.joystickSettings.bombInput },

        { "input-settings-switch-screen-left", settings.joystickSettings.switchScreenLeft },
        { "input-settings-switch-screen-right", settings.joystickSettings.switchScreenRight },
        { "input-settings-pause", settings.joystickSettings.pauseInput },
        { "input-settings-ok", settings.joystickSettings.okInput },
        { "input-settings-cancel", settings.joystickSettings.cancelInput },
    };

    for (const auto& mapping : JoystickMappings)
    {
        mappingButtons.emplace_back(std::make_unique<UIInputRemapper>(inputManager, mapping.source, lm, true));
        auto& button = mappingButtons.back();

        createCommonInputRemapper(*button, rm, lm, "ui-select-field.png", "ui-select-field.png",
            sf::FloatRect(16, 0, 8, 1), sf::FloatRect(0, 0, FrameWidth - 2 * ButtonSpace, ButtonHeight),
            mapping.langID, ButtonCaptionSize, sf::Color::White, 1, sf::Color::Black, sf::Vector2f(24, 0));
        button->setPosition(pos);

        button->getPressedSprite()->setBlendColor(sf::Color::Yellow);
        button->getActiveSprite()->setOpacity(0.5);
        button->getActiveSprite()->setOpacity(0.5);
        button->setDepth(3200);

        pos += sf::Vector2f(0, ButtonHeight + ButtonSpace);
    }

    mappingButtons.shrink_to_fit();

    buttonGroup.setGroupingFunction([](size_t i)
    {
        if (i >= 0 && i <= 2) return std::make_pair(0, 2);
        else if (i >= 3 && i <= 7) return std::make_pair(3, 7);
        else return std::make_pair(8, 11);
    });
}

void InputMappingSettingsPanel::update(FrameTime curTime)
{

}

void InputMappingSettingsPanel::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(getCenterPosition());
    renderer.currentTransform.translate(0, -TotalHeight/2 + ButtonHeight/2);
    renderer.pushDrawable(title, {}, 3200);
    renderer.popTransform();

    for (auto& btn : mappingButtons) btn->render(renderer);
    backButton.render(renderer);
}

void InputMappingSettingsPanel::activate()
{
    for (auto& btn : mappingButtons) btn->activate();
    backButton.activate();
    buttonGroup.activate();
}

void InputMappingSettingsPanel::deactivate()
{
    for (auto& btn : mappingButtons) btn->deactivate();
    backButton.deactivate();
    buttonGroup.deactivate();
}
