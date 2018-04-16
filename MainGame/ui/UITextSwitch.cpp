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


#include "UITextSwitch.hpp"

#include "UIButtonGroup.hpp"
#include "UIButtonCommons.hpp"

#include "rendering/Renderer.hpp"
#include <rectUtils.hpp>
#include <cmath>

#include "resources/ResourceManager.hpp"
#include "language/LocalizationManager.hpp"
#include "language/convenienceConfigText.hpp"

UITextSwitch::UITextSwitch(InputManager& inputManager, const InputSettings& settings, bool* target) : UITextSwitch(target)
{
    initialize(inputManager, settings);
}

void UITextSwitch::initialize(InputManager& inputManager, const InputSettings& settings)
{
    UIButton::initialize(inputManager);
    
    switchAxis.registerButton(inputManager, settings.keyboardSettings.moveLeft,  AxisDirection::Negative, 0);
    switchAxis.registerButton(inputManager, settings.keyboardSettings.moveRight, AxisDirection::Positive, 0);
    switchAxis.registerAxis(inputManager, settings.joystickSettings.movementAxisX, 0);
    switchAxis.registerAxis(inputManager, settings.joystickSettings.movementAxisXAlt, 1);
    
    setPressAction([=]
    { 
        if (switchTarget)
        {
            *switchTarget = !*switchTarget;
            if (switchAction) switchAction(*switchTarget);
            resetText();
        }
    });
    
    switchAxis.setAction([=](float val)
    {
        if (val > -0.5 && val < 0.5) return;
        if (!parentGroup || this != parentGroup->getCurrentButton()) return;
        getPressAction()();
    });
}

void UITextSwitch::resetText()
{
    if (switchTarget) switchCaption.setString(*switchTarget ? trueString : falseString);
    else switchCaption.setString("");
    switchCaption.buildGeometry();
}

void UITextSwitch::render(Renderer& renderer)
{
    UIButton::render(renderer);
    
    renderer.pushTransform();
    renderer.currentTransform.translate(getPosition());

    auto bounds = getBounds();
    auto captionDisplacement = getCaptionDisplacement();
    
    renderer.currentTransform.translate(floorf(bounds.left + bounds.width/2), floorf(bounds.top + bounds.height/2));
    renderer.pushDrawable(switchCaption, {}, getDepth());
    
    renderer.popTransform();
}

void createCommonTextSwitch(UITextSwitch& button, ResourceManager& rm, LocalizationManager& lm,
    std::string activeResourceName, std::string pressedResourceName, sf::FloatRect centerRect,
    sf::FloatRect destRect, LangID captionString, size_t captionSize, sf::Color textColor,
    float outlineThickness, sf::Color outlineColor, sf::Vector2f captionDisplacement)
{
    createCommonTextualButton(button, rm, lm, activeResourceName, pressedResourceName, centerRect, destRect,
        captionString, captionSize, textColor, outlineThickness, outlineColor, captionDisplacement);
    
    auto& caption = button.getSwitchCaption();
    caption.setFontHandler(rm.load<FontHandler>(lm.getFontName()));
    caption.setFontSize(captionSize);
    caption.setDefaultColor(textColor);
    caption.setOutlineThickness(outlineThickness);
    caption.setDefaultOutlineColor(outlineColor);
    caption.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    caption.setVerticalAnchor(TextDrawable::VertAnchor::Center);
    configTextDrawable(caption, lm);
    
    caption.setWordWrappingWidth(destRect.width - 2 * captionDisplacement.x);
    caption.setWordAlignment(TextDrawable::Alignment::Inverse);
    
    button.setTrueString(lm.getString("ui-switch-yes"));
    button.setFalseString(lm.getString("ui-switch-no"));
    button.resetText();
}
