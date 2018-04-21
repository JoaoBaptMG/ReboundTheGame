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


#include "UIInputRemapper.hpp"

#include "UIButtonGroup.hpp"
#include "UIButtonCommons.hpp"

#include "rendering/Renderer.hpp"
#include <rectUtils.hpp>
#include <cmath>
#include <assert.h>

#include "resources/ResourceManager.hpp"
#include "language/LocalizationManager.hpp"
#include "language/convenienceConfigText.hpp"
#include "language/KeyboardKeyName.hpp"

#include "audio/AudioManager.hpp"
#include "audio/Sound.hpp"

UIInputRemapper::UIInputRemapper(Services& services, InputSource& source, UIInputRemapper::InputDest inputDest)
    : UIInputRemapper(source, services.localizationManager, inputDest)
{
    initialize(services);
}

void UIInputRemapper::initialize(Services& services)
{
    UIButton::initialize(services.inputManager);
    
    setPressAction([&services,this] ()
    {
        auto parentGroup = static_cast<UIInputRemappingButtonGroup*>(this->parentGroup);
        parentGroup->setRemappingButton(this);

        playConfirm(services);
        auto callback = [=,&services](InputSource source, float value)
        {
            if (value < 0.5f)
            {
                services.audioManager.playSound(services.resourceManager.load<Sound>("ui-keybind.wav"));
                parentGroup->assignRemappingUniquely(*this, source);
                parentGroup->setRemappingButton(nullptr);
            }
        };

        if (inputDest == InputDest::Keyboard) services.inputManager.addPickAllKeyboardCallback(callback);
        else services.inputManager.addPickAllJoystickCallback(callback, inputDest != InputDest::JoystickButton);

        static const LangID RemapStrings[] = { "input-settings-remap", "input-settings-remap",
            "input-settings-hor-axis-remap", "input-settings-vert-axis-remap" };
        sourceCaption.setString(localizationManager.getString(RemapStrings[(size_t)inputDest]));
        sourceCaption.buildGeometry();
    });
}

void UIInputRemapper::resetText()
{
    sourceCaption.setString(remapSource.getInputName(localizationManager));
    sourceCaption.buildGeometry();
}

void UIInputRemapper::resetRemappingState(InputManager& inputManager)
{
    resetText();

    if (inputDest == InputDest::Keyboard) inputManager.addPickAllKeyboardCallback(InputManager::Callback());
    else inputManager.addPickAllJoystickCallback(InputManager::Callback());
}

void UIInputRemapper::render(Renderer& renderer)
{
    UIButton::render(renderer);
    
    renderer.pushTransform();
    renderer.currentTransform.translate(getPosition());

    auto bounds = getBounds();
    auto captionDisplacement = getCaptionDisplacement();
    
    renderer.currentTransform.translate(floorf(bounds.left + bounds.width/2), floorf(bounds.top + bounds.height/2));
    renderer.pushDrawable(sourceCaption, {}, getDepth());
    
    renderer.popTransform();
}

void createCommonInputRemapper(UIInputRemapper& button, Services& services,
    std::string activeResourceName, std::string pressedResourceName, sf::FloatRect centerRect,
    sf::FloatRect destRect, LangID captionString, size_t captionSize, sf::Color textColor,
    float outlineThickness, sf::Color outlineColor, sf::Vector2f captionDisplacement)
{
    createCommonTextualButton(button, services, activeResourceName, pressedResourceName, centerRect, destRect,
        captionString, captionSize, textColor, outlineThickness, outlineColor, captionDisplacement);
    
    auto& caption = button.getSourceCaption();
    caption.setFontHandler(loadDefaultFont(services));
    caption.setFontSize(captionSize);
    caption.setDefaultColor(textColor);
    caption.setOutlineThickness(outlineThickness);
    caption.setDefaultOutlineColor(outlineColor);
    caption.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    caption.setVerticalAnchor(TextDrawable::VertAnchor::Center);
    configTextDrawable(caption, services.localizationManager);
    
    caption.setWordWrappingWidth(destRect.width - 2 * captionDisplacement.x);
    caption.setWordAlignment(TextDrawable::Alignment::Inverse);
    button.resetText();
}

UIInputRemappingButtonGroup::UIInputRemappingButtonGroup(Services& services, TravelingMode travelingMode)
    : UIButtonGroup(services, travelingMode), inputManager(services.inputManager),
    currentRemappingButton(nullptr) {}

void UIInputRemappingButtonGroup::setRemappingButton(UIInputRemapper* newRemappingButton)
{
    if (currentRemappingButton == newRemappingButton) return;
    else if (currentRemappingButton != nullptr) currentRemappingButton->resetRemappingState(inputManager);
    currentRemappingButton = newRemappingButton;
}

void UIInputRemappingButtonGroup::setGroupingFunction(decltype(groupingFunction) func) { groupingFunction = func; }

void UIInputRemappingButtonGroup::assignRemappingUniquely(UIInputRemapper& button, InputSource newSource)
{
    if (groupingFunction)
    {
        auto& buttons = getButtons();
        auto id = std::find(buttons.begin(), buttons.end(), &button) - buttons.begin();
        auto interval = groupingFunction(id);

        for (size_t i = interval.first; i <= interval.second; i++)
            if (auto btn = reinterpret_cast<UIInputRemapper*>(buttons[i]))
                if (btn->remapSource == newSource)
                {
                    btn->remapSource = button.remapSource;
                    btn->resetText();
                }
    }

    button.remapSource = newSource;
}
