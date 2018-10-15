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

#include "RootSettingsPanel.hpp"

#include "SettingsScene.hpp"
#include "InputMappingSettingsPanel.hpp"
#include "LanguageSelectSettingsPanel.hpp"

#include "resources/ResourceManager.hpp"
#include "language/LocalizationManager.hpp"
#include "language/convenienceConfigText.hpp"
#include "rendering/Renderer.hpp"

#include "ui/UIButtonCommons.hpp"
#include "ColorList.hpp"

#include <defaults.hpp>

constexpr float FrameWidth = PlayfieldWidth;
constexpr float ButtonHeight = 36;
constexpr float ButtonSpace = 8;
constexpr float ButtonCaptionSize = 28;
constexpr float TotalHeight = 11 * ButtonHeight + 10 * ButtonSpace;

const LangID Titles[] =
{
    "settings-video-settings",
    "settings-audio-settings",
    "settings-input-settings",
};

const LangID VideoIdentifiers[] =
{
    "settings-fullscreen",
    "settings-vsync",
    "settings-change-language",
};

const LangID SoundIdentifiers[] =
{
    "settings-music-volume",
    "settings-sound-volume",
};


const LangID InputIdentifiers[] =
{
    "settings-keyboard-controls",
    "settings-joystick-controls",
};

extern bool GlobalUpdateWindowHandler;

glm::vec2 SettingsPanel::getCenterPosition() const { return curSettings->centerPosition; }
void SettingsPanel::executeBackAction() { if (curSettings->backAction) curSettings->backAction(); }
const LangID& SettingsPanel::getBackId() const { return curSettings->backId; }

RootSettingsPanel::RootSettingsPanel(Services& services, SettingsBase* curSettings, UIPointer& pointer)
    : SettingsPanel(curSettings, pointer),
     fullscreen(services.settings.videoSettings.fullscreen),
     vsync(services.settings.videoSettings.vsyncEnabled),
     musicVolume(services.settings.audioSettings.musicVolume),
     soundVolume(services.settings.audioSettings.soundVolume),
     fullscreenSwitch(services, &fullscreen),
     vsyncSwitch(services, &vsync),
     languageButton(services.inputManager),
     musicSlider(services, &musicVolume, 100),
     soundSlider(services, &soundVolume, 100),
     keyboardButton(services.inputManager), joystickButton(services.inputManager),
     backButton(services.inputManager), buttonGroup(services)
{
    auto pos = getCenterPosition() + glm::vec2(0, -TotalHeight/2 + ButtonHeight/2);

    size_t k = 0;
    for (auto& title : titles)
    {
        title.setFontHandler(loadDefaultFont(services));
        title.setString(services.localizationManager.getString(Titles[k]));
        title.setFontSize(ButtonCaptionSize);
        title.setDefaultColor(Colors::Yellow);
        title.setOutlineThickness(1);
        title.setDefaultOutlineColor(Colors::Black);
        title.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
        title.setVerticalAnchor(TextDrawable::VertAnchor::Center);
        configTextDrawable(title, services.localizationManager);
        title.buildGeometry();
        k++;
    }

    k = 0;
    for (auto button : { &fullscreenSwitch, &vsyncSwitch })
    {
        createCommonTextSwitch(*button, services, "ui-select-field.png", "ui-select-field.png",
            util::rect(16, 0, 8, 1), util::rect(0, 0, FrameWidth - 2 * ButtonSpace, ButtonHeight),
            VideoIdentifiers[k], ButtonCaptionSize, Colors::White, 1, Colors::Black, glm::vec2(24, 0));
        button->setPosition(pos + glm::vec2(0, (k+1) * (ButtonHeight + ButtonSpace)));
        k++;
    }

    createCommonTextualButton(languageButton, services, "ui-select-field.png", "ui-select-field.png",
        util::rect(16, 0, 8, 1), util::rect(0, 0, FrameWidth - 2 * ButtonSpace, ButtonHeight),
        VideoIdentifiers[2], ButtonCaptionSize, Colors::White, 1, Colors::Black, glm::vec2(24, 0));
    languageButton.setPosition(pos + glm::vec2(0, 3 * (ButtonHeight + ButtonSpace)));

    k = 0;
    for (auto button : { &musicSlider, &soundSlider })
    {
        createCommonTextualButton(*button, services, "ui-select-field.png", "ui-select-field.png",
            util::rect(16, 0, 8, 1), util::rect(0, 0, FrameWidth - 2 * ButtonSpace, ButtonHeight),
            SoundIdentifiers[k], ButtonCaptionSize, Colors::White, 1, Colors::Black, glm::vec2(24, 0));
        button->setPosition(pos + glm::vec2(0, (k+5) * (ButtonHeight + ButtonSpace)));
        k++;
    }

    k = 0;
    for (auto button : { &keyboardButton, &joystickButton })
    {
        createCommonTextualButton(*button, services, "ui-select-field.png", "ui-select-field.png",
            util::rect(16, 0, 8, 1), util::rect(0, 0, FrameWidth - 2 * ButtonSpace, ButtonHeight),
            InputIdentifiers[k], ButtonCaptionSize, Colors::White, 1, Colors::Black, glm::vec2(24, 0));
        button->setPosition(pos + glm::vec2(0, (k+8) * (ButtonHeight + ButtonSpace)));
        k++;
    }

    createCommonTextualButton(backButton, services, "ui-select-field.png", "ui-select-field.png",
        util::rect(16, 0, 8, 1), util::rect(0, 0, FrameWidth - 2 * ButtonSpace, ButtonHeight),
        getBackId(), ButtonCaptionSize, Colors::White, 1, Colors::Black, glm::vec2(24, 0),
        TextDrawable::Alignment::Center);
    backButton.setPosition(pos + glm::vec2(0, 10 * (ButtonHeight + ButtonSpace)));

    for (auto button : std::initializer_list<UIButton*>{ &fullscreenSwitch, &vsyncSwitch, &languageButton, &musicSlider,
                                                         &soundSlider, &keyboardButton, &joystickButton, &backButton })
    {
        button->getPressedSprite()->setBlendColor(Colors::Yellow);
        button->getActiveSprite()->setOpacity(0.5);
        button->getActiveSprite()->setOpacity(0.5);
        button->setDepth(5000);
    }

    fullscreenSwitch.setSwitchAction([&] (bool val)
    {
        services.settings.videoSettings.fullscreen = val;
        GlobalUpdateWindowHandler = true;
    });

    vsyncSwitch.setSwitchAction([&] (bool val)
    {
        services.settings.videoSettings.vsyncEnabled = val;
        GlobalUpdateWindowHandler = true;
    });

    languageButton.setPressAction([&, curSettings = this->curSettings]
    {
        playConfirm(services);
        curSettings->changeSettingsPanel(new LanguageSelectSettingsPanel(services, curSettings, pointer));
    });

    musicSlider.setSlideAction([&] (size_t val) { services.settings.audioSettings.musicVolume = val; });
    soundSlider.setSlideAction([&] (size_t val) { services.settings.audioSettings.soundVolume = val; });

    keyboardButton.setPressAction([&, curSettings = this->curSettings]
    {
        playConfirm(services);
        curSettings->changeSettingsPanel(new InputMappingSettingsPanel(services, curSettings, pointer, false));
    });

    joystickButton.setPressAction([&, curSettings = this->curSettings]
    {
        playConfirm(services);
        curSettings->changeSettingsPanel(new InputMappingSettingsPanel(services, curSettings, pointer, true));
    });

    backButton.setPressAction([=] { executeBackAction(); });

    buttonGroup.setButtons({ fullscreenSwitch, vsyncSwitch, languageButton, musicSlider, soundSlider, keyboardButton,
                             joystickButton, backButton });
    buttonGroup.setPointer(pointer);
}

void RootSettingsPanel::update(FrameTime curTime)
{
    musicSlider.update();
    soundSlider.update();
}

void RootSettingsPanel::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform *= util::translate(getCenterPosition());
    renderer.currentTransform *= util::translate(0, -TotalHeight/2 + ButtonHeight/2);
    int k = 0;
    for (auto& title : titles)
    {
        renderer.pushDrawable(title, 5000);
        renderer.currentTransform *= util::translate(0, (4-(k++)) * (ButtonHeight + ButtonSpace));
    }
    renderer.popTransform();

    fullscreenSwitch.render(renderer);
    vsyncSwitch.render(renderer);
    languageButton.render(renderer);

    musicSlider.render(renderer);
    soundSlider.render(renderer);

    keyboardButton.render(renderer);
    joystickButton.render(renderer);
    backButton.render(renderer);
}

void RootSettingsPanel::activate()
{
    fullscreenSwitch.activate();
    vsyncSwitch.activate();

    musicSlider.activate();
    soundSlider.activate();

    languageButton.activate();
    keyboardButton.activate();
    joystickButton.activate();
    backButton.activate();

    buttonGroup.activate();
}

void RootSettingsPanel::deactivate()
{
    fullscreenSwitch.deactivate();
    vsyncSwitch.deactivate();

    musicSlider.deactivate();
    soundSlider.deactivate();

    languageButton.deactivate();
    keyboardButton.deactivate();
    joystickButton.deactivate();
    backButton.deactivate();

    buttonGroup.deactivate();
}
