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

sf::Vector2f SettingsPanel::getCenterPosition() const { return curSettings->centerPosition; }
void SettingsPanel::executeBackAction() { if (curSettings->backAction) curSettings->backAction(); }
const LangID& SettingsPanel::getBackId() const { return curSettings->backId; }

RootSettingsPanel::RootSettingsPanel(Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager &lm,
    SettingsBase* curSettings, UIPointer& pointer) : SettingsPanel(curSettings, pointer),
     fullscreen(settings.videoSettings.fullscreen), vsync(settings.videoSettings.vsyncEnabled),
     musicVolume(settings.audioSettings.musicVolume), soundVolume(settings.audioSettings.soundVolume),
     fullscreenSwitch(im, settings.inputSettings, &fullscreen), vsyncSwitch(im, settings.inputSettings, &vsync),
     languageButton(im), musicSlider(im, rm, settings.inputSettings, lm.isRTL(), &musicVolume, 100),
     soundSlider(im, rm, settings.inputSettings, lm.isRTL(), &soundVolume, 100),
     keyboardButton(im), joystickButton(im), backButton(im), buttonGroup(im, settings.inputSettings)
{
    auto pos = getCenterPosition() + sf::Vector2f(0, -TotalHeight/2 + ButtonHeight/2);

    size_t k = 0;
    for (auto& title : titles)
    {
        title.setFontHandler(rm.load<FontHandler>(lm.getFontName()));
        title.setString(lm.getString(Titles[k]));
        title.setFontSize(ButtonCaptionSize);
        title.setDefaultColor(sf::Color::Yellow);
        title.setOutlineThickness(1);
        title.setDefaultOutlineColor(sf::Color::Black);
        title.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
        title.setVerticalAnchor(TextDrawable::VertAnchor::Center);
        configTextDrawable(title, lm);
        title.buildGeometry();
        k++;
    }

    k = 0;
    for (auto button : { &fullscreenSwitch, &vsyncSwitch })
    {
        createCommonTextSwitch(*button, rm, lm, "ui-select-field.png", "ui-select-field.png",
            sf::FloatRect(16, 0, 8, 1), sf::FloatRect(0, 0, FrameWidth - 2 * ButtonSpace, ButtonHeight),
            VideoIdentifiers[k], ButtonCaptionSize, sf::Color::White, 1, sf::Color::Black, sf::Vector2f(24, 0));
        button->setPosition(pos + sf::Vector2f(0, (k+1) * (ButtonHeight + ButtonSpace)));
        k++;
    }

    createCommonTextualButton(languageButton, rm, lm, "ui-select-field.png", "ui-select-field.png",
        sf::FloatRect(16, 0, 8, 1), sf::FloatRect(0, 0, FrameWidth - 2 * ButtonSpace, ButtonHeight),
        VideoIdentifiers[2], ButtonCaptionSize, sf::Color::White, 1, sf::Color::Black, sf::Vector2f(24, 0));
    languageButton.setPosition(pos + sf::Vector2f(0, 3 * (ButtonHeight + ButtonSpace)));

    k = 0;
    for (auto button : { &musicSlider, &soundSlider })
    {
        createCommonTextualButton(*button, rm, lm, "ui-select-field.png", "ui-select-field.png",
            sf::FloatRect(16, 0, 8, 1), sf::FloatRect(0, 0, FrameWidth - 2 * ButtonSpace, ButtonHeight),
            SoundIdentifiers[k], ButtonCaptionSize, sf::Color::White, 1, sf::Color::Black, sf::Vector2f(24, 0));
        button->setPosition(pos + sf::Vector2f(0, (k+5) * (ButtonHeight + ButtonSpace)));
        k++;
    }

    k = 0;
    for (auto button : { &keyboardButton, &joystickButton })
    {
        createCommonTextualButton(*button, rm, lm, "ui-select-field.png", "ui-select-field.png",
            sf::FloatRect(16, 0, 8, 1), sf::FloatRect(0, 0, FrameWidth - 2 * ButtonSpace, ButtonHeight),
            InputIdentifiers[k], ButtonCaptionSize, sf::Color::White, 1, sf::Color::Black, sf::Vector2f(24, 0));
        button->setPosition(pos + sf::Vector2f(0, (k+8) * (ButtonHeight + ButtonSpace)));
        k++;
    }

    createCommonTextualButton(backButton, rm, lm, "ui-select-field.png", "ui-select-field.png",
        sf::FloatRect(16, 0, 8, 1), sf::FloatRect(0, 0, FrameWidth - 2 * ButtonSpace, ButtonHeight),
        getBackId(), ButtonCaptionSize, sf::Color::White, 1, sf::Color::Black, sf::Vector2f(24, 0),
        TextDrawable::Alignment::Center);
    backButton.setPosition(pos + sf::Vector2f(0, 10 * (ButtonHeight + ButtonSpace)));

    for (auto button : std::initializer_list<UIButton*>{ &fullscreenSwitch, &vsyncSwitch, &languageButton, &musicSlider,
                                                         &soundSlider, &keyboardButton, &joystickButton, &backButton })
    {
        button->getPressedSprite()->setBlendColor(sf::Color::Yellow);
        button->getActiveSprite()->setOpacity(0.5);
        button->getActiveSprite()->setOpacity(0.5);
        button->setDepth(3200);
    }

    fullscreenSwitch.setSwitchAction([&] (bool val)
    {
        settings.videoSettings.fullscreen = val;
        GlobalUpdateWindowHandler = true;
    });

    vsyncSwitch.setSwitchAction([&] (bool val)
    {
        settings.videoSettings.vsyncEnabled = val;
        GlobalUpdateWindowHandler = true;
    });

    languageButton.setPressAction([&, curSettings = this->curSettings]
    {
        curSettings->changeSettingsPanel(new LanguageSelectSettingsPanel(settings, im, rm, lm, curSettings, pointer));
    });

    musicSlider.setSlideAction([&] (size_t val) { settings.audioSettings.musicVolume = val; });
    soundSlider.setSlideAction([&] (size_t val) { settings.audioSettings.soundVolume = val; });

    keyboardButton.setPressAction([&, curSettings = this->curSettings]
    {
        curSettings->changeSettingsPanel(new InputMappingSettingsPanel(settings, im, rm, lm, curSettings, pointer, false));
    });

    joystickButton.setPressAction([&, curSettings = this->curSettings]
    {
        curSettings->changeSettingsPanel(new InputMappingSettingsPanel(settings, im, rm, lm, this->curSettings, pointer, true));
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
    renderer.currentTransform.translate(getCenterPosition());
    renderer.currentTransform.translate(0, -TotalHeight/2 + ButtonHeight/2);
    int k = 0;
    for (auto& title : titles)
    {
        renderer.pushDrawable(title, {}, 3200);
        renderer.currentTransform.translate(0, (4-(k++)) * (ButtonHeight + ButtonSpace));
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

    keyboardButton.deactivate();
    joystickButton.deactivate();
    backButton.deactivate();

    buttonGroup.deactivate();
}