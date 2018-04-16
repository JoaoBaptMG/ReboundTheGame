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


#include "LanguageSelectSettingsPanel.hpp"

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
constexpr float ButtonSpace = 8;
constexpr float ButtonCaptionSize = 28;
constexpr float SmallButtonSpace = 4;
constexpr float SmallButtonHeight = 24;
constexpr float SmallCaptionSize = 20;
constexpr float TotalHeight = 472;

LanguageSelectSettingsPanel::LanguageSelectSettingsPanel(Settings& settings, InputManager& im, ResourceManager& rm,
    LocalizationManager &lm, SettingsBase* curSettings, UIPointer& pointer) : SettingsPanel(curSettings, pointer),
    backButton(im), buttonGroup(im, settings.inputSettings)
{
    auto pos = getCenterPosition() + sf::Vector2f(0, -TotalHeight/2 + ButtonHeight/2);

    title.setFontHandler(rm.load<FontHandler>(lm.getFontName()));
    title.setString(lm.getString("settings-language-select-title"));
    title.setFontSize(ButtonCaptionSize);
    title.setDefaultColor(sf::Color::Yellow);
    title.setOutlineThickness(1);
    title.setDefaultOutlineColor(sf::Color::Black);
    title.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    title.setVerticalAnchor(TextDrawable::VertAnchor::Center);
    configTextDrawable(title, lm);
    title.buildGeometry();

    pos += sf::Vector2f(0, ButtonHeight + ButtonSpace);

    for (std::string lang : getAllLanguageDescriptors())
    {
        languageButtons.emplace_back(std::make_unique<UIButton>(im));
        auto& button = languageButtons.back();

        createCommonTextualButton(*button, rm, lm, "ui-select-field.png", "ui-select-field.png",
            sf::FloatRect(16, 0, 8, 1), sf::FloatRect(0, 0, FrameWidth - 2 * ButtonSpace, SmallButtonHeight),
            "", SmallCaptionSize, sf::Color::White, 1, sf::Color::Black, sf::Vector2f(24, 0),
            TextDrawable::Alignment::Direct);
        button->getCaption()->setString(getLanguageDescriptorName(lang));
        button->getCaption()->buildGeometry();

        button->setPosition(pos);

        button->getPressedSprite()->setBlendColor(sf::Color::Yellow);
        button->getActiveSprite()->setOpacity(0.5);
        button->getActiveSprite()->setOpacity(0.5);
        button->setDepth(3200);

        button->setPressAction([&, lang, curSettings = this->curSettings]
        {
            lm.loadLanguageDescriptor(settings.languageFile = lang);
            curSettings->changeSettingsPanel(new RootSettingsPanel(settings, im, rm, lm, curSettings, pointer));
        });

        pos += sf::Vector2f(0, SmallButtonHeight + SmallButtonSpace);
    }

    languageButtons.shrink_to_fit();

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
        curSettings->changeSettingsPanel(new RootSettingsPanel(settings, im, rm, lm, curSettings, pointer));
    });

    std::vector<UIButton*> buttons;
    for (auto& button : languageButtons) buttons.push_back(button.get());
    buttons.push_back(&backButton);
    buttonGroup.setButtons(buttons);
    buttonGroup.setPointer(pointer);
}

void LanguageSelectSettingsPanel::update(FrameTime curTime)
{

}

void LanguageSelectSettingsPanel::render(Renderer &renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(getCenterPosition());
    renderer.currentTransform.translate(0, -TotalHeight/2 + ButtonHeight/2);
    renderer.pushDrawable(title, {}, 3200);
    renderer.popTransform();

    for (auto& btn : languageButtons) btn->render(renderer);
    backButton.render(renderer);
}

void LanguageSelectSettingsPanel::activate()
{
    for (auto& btn : languageButtons) btn->activate();
    backButton.activate();
    buttonGroup.activate();
}

void LanguageSelectSettingsPanel::deactivate()
{
    for (auto& btn : languageButtons) btn->deactivate();
    backButton.deactivate();
    buttonGroup.deactivate();
}
