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
#include "ColorList.hpp"

#include <defaults.hpp>

#include <array>

constexpr float FrameWidth = PlayfieldWidth;
constexpr float ButtonHeight = 32;
constexpr float ButtonSpace = 4;
constexpr float ButtonCaptionSize = 28;
constexpr float TotalHeight = 500;

struct InputMapping
{
    LangID langID;
    InputSource& source;
    UIInputRemapper::InputDest inputDest;
};

InputMappingSettingsPanel::InputMappingSettingsPanel(Services& services, SettingsBase* curSettings,
    UIPointer& pointer, bool forJoystick) : SettingsPanel(curSettings, pointer),
    backButton(services.inputManager), buttonGroup(services)
{
    auto pos = getCenterPosition() + glm::vec2(0, -TotalHeight/2 + ButtonHeight/2);

    title.setFontHandler(loadDefaultFont(services));
    title.setString(services.localizationManager.getString(forJoystick ? "settings-joystick-title" : "settings-keyboard-title"));
    title.setFontSize(ButtonCaptionSize);
    title.setDefaultColor(Colors::Yellow);
    title.setOutlineThickness(1);
    title.setDefaultOutlineColor(Colors::Black);
    title.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    title.setVerticalAnchor(TextDrawable::VertAnchor::Center);
    configTextDrawable(title, services.localizationManager);
    title.buildGeometry();

    pos += glm::vec2(0, ButtonHeight + ButtonSpace);
    buildInputScreen(pos, services, forJoystick);

    createCommonTextualButton(backButton, services, "ui-select-field.png", "ui-select-field.png",
        util::rect(16, 0, 8, 1), util::rect(0, 0, FrameWidth - 2 * ButtonSpace, ButtonHeight),
        "settings-back-to-root", ButtonCaptionSize, Colors::White, 1, Colors::Black, glm::vec2(24, 0),
        TextDrawable::Alignment::Center);
    backButton.setPosition(getCenterPosition() + glm::vec2(0, TotalHeight/2 - ButtonHeight/2));

    backButton.getPressedSprite()->setBlendColor(Colors::Yellow);
    backButton.getActiveSprite()->setOpacity(0.5);
    backButton.getActiveSprite()->setOpacity(0.5);
    backButton.setDepth(5000);

    backButton.setPressAction([&, curSettings = this->curSettings]
    {
        playConfirm(services);

        if (forJoystick) services.inputManager.addPickAllJoystickCallback(InputManager::Callback());
        else services.inputManager.addPickAllKeyboardCallback(InputManager::Callback());

        curSettings->changeSettingsPanel(new RootSettingsPanel(services, curSettings, pointer));
    });

    std::vector<UIButton*> buttons;
    for (auto& button : mappingButtons) buttons.push_back(button.get());
    buttons.push_back(&backButton);
    buttonGroup.setButtons(buttons);
    buttonGroup.setPointer(pointer);
}

void InputMappingSettingsPanel::buildInputScreen(glm::vec2 pos, Services& services, bool forJoystick)
{
    auto& settings = services.settings.inputSettings;

    std::array<InputMapping,12> inputMappings = !forJoystick ? std::array<InputMapping,12>
    {
        InputMapping{ "input-settings-dash", settings.keyboardSettings.dashInput, UIInputRemapper::InputDest::Keyboard },
        InputMapping{ "input-settings-jump", settings.keyboardSettings.jumpInput, UIInputRemapper::InputDest::Keyboard },
        InputMapping{ "input-settings-bomb", settings.keyboardSettings.bombInput, UIInputRemapper::InputDest::Keyboard },

        InputMapping{ "input-settings-keyboard-left", settings.keyboardSettings.moveLeft, UIInputRemapper::InputDest::Keyboard },
        InputMapping{ "input-settings-keyboard-right", settings.keyboardSettings.moveRight, UIInputRemapper::InputDest::Keyboard },
        InputMapping{ "input-settings-keyboard-up", settings.keyboardSettings.moveUp, UIInputRemapper::InputDest::Keyboard },
        InputMapping{ "input-settings-keyboard-down", settings.keyboardSettings.moveDown, UIInputRemapper::InputDest::Keyboard },

        InputMapping{ "input-settings-switch-screen-left", settings.keyboardSettings.switchScreenLeft, UIInputRemapper::InputDest::Keyboard },
        InputMapping{ "input-settings-switch-screen-right", settings.keyboardSettings.switchScreenRight, UIInputRemapper::InputDest::Keyboard },
        InputMapping{ "input-settings-pause", settings.keyboardSettings.pauseInput, UIInputRemapper::InputDest::Keyboard },
        InputMapping{ "input-settings-ok", settings.keyboardSettings.okInput, UIInputRemapper::InputDest::Keyboard },
        InputMapping{ "input-settings-cancel", settings.keyboardSettings.cancelInput, UIInputRemapper::InputDest::Keyboard },
    } : std::array<InputMapping,12>
    {
        InputMapping{ "input-settings-dash", settings.joystickSettings.dashInput, UIInputRemapper::InputDest::JoystickButton },
        InputMapping{ "input-settings-jump", settings.joystickSettings.jumpInput, UIInputRemapper::InputDest::JoystickButton },
        InputMapping{ "input-settings-bomb", settings.joystickSettings.bombInput, UIInputRemapper::InputDest::JoystickButton },

        InputMapping{ "input-settings-joystick-hor-movement", settings.joystickSettings.movementAxisX, UIInputRemapper::InputDest::HorJoystickAxis },
        InputMapping{ "input-settings-joystick-vert-movement", settings.joystickSettings.movementAxisY, UIInputRemapper::InputDest::VertJoystickAxis },
        InputMapping{ "input-settings-joystick-hor-movement-2", settings.joystickSettings.movementAxisXAlt, UIInputRemapper::InputDest::HorJoystickAxis },
        InputMapping{ "input-settings-joystick-vert-movement-2", settings.joystickSettings.movementAxisYAlt, UIInputRemapper::InputDest::VertJoystickAxis },

        InputMapping{ "input-settings-switch-screen-left", settings.joystickSettings.switchScreenLeft, UIInputRemapper::InputDest::JoystickButton },
        InputMapping{ "input-settings-switch-screen-right", settings.joystickSettings.switchScreenRight, UIInputRemapper::InputDest::JoystickButton },
        InputMapping{ "input-settings-pause", settings.joystickSettings.pauseInput, UIInputRemapper::InputDest::JoystickButton },
        InputMapping{ "input-settings-ok", settings.joystickSettings.okInput, UIInputRemapper::InputDest::JoystickButton },
        InputMapping{ "input-settings-cancel", settings.joystickSettings.cancelInput, UIInputRemapper::InputDest::JoystickButton },
    };

    for (const auto& mapping : inputMappings)
    {
        mappingButtons.emplace_back(std::make_unique<UIInputRemapper>(services, mapping.source, mapping.inputDest));
        auto& button = mappingButtons.back();

        createCommonInputRemapper(*button, services, "ui-select-field.png", "ui-select-field.png",
            util::rect(16, 0, 8, 1), util::rect(0, 0, FrameWidth - 2 * ButtonSpace, ButtonHeight),
            mapping.langID, ButtonCaptionSize, Colors::White, 1, Colors::Black, glm::vec2(24, 0));
        button->setPosition(pos);

        button->getPressedSprite()->setBlendColor(Colors::Yellow);
        button->getActiveSprite()->setOpacity(0.5);
        button->getActiveSprite()->setOpacity(0.5);
        button->setDepth(5000);

        pos += glm::vec2(0, ButtonHeight + ButtonSpace);
    }

    mappingButtons.shrink_to_fit();

    buttonGroup.setGroupingFunction([](size_t i)
    {
        if (i >= 0 && i <= 2) return std::make_pair(0, 2);
        else if (i >= 3 && i <= 6) return std::make_pair(3, 6);
        else return std::make_pair(7, 11);
    });
}

void InputMappingSettingsPanel::update(FrameTime curTime)
{

}

void InputMappingSettingsPanel::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform *= util::translate(getCenterPosition());
    renderer.currentTransform *= util::translate(0, -TotalHeight/2 + ButtonHeight/2);
    renderer.pushDrawable(title, 5000);
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
