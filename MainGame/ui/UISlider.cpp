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


#include "UISlider.hpp"

#include "UIButtonGroup.hpp"
#include "UIButtonCommons.hpp"

#include "rendering/Renderer.hpp"
#include <rectUtils.hpp>
#include <cmath>
#include <algorithm>

#include "resources/ResourceManager.hpp"
#include "language/LocalizationManager.hpp"
#include "language/convenienceConfigText.hpp"

UISlider::UISlider(InputManager& inputManager, ResourceManager& resourceManager, const InputSettings& settings,
    bool rtl, size_t* target, size_t max) : UISlider(target, max)
{
    initialize(inputManager, resourceManager, settings, rtl);
}

void UISlider::initialize(InputManager& inputManager, ResourceManager& resourceManager, const InputSettings& settings,
    bool rtl)
{
    UIButton::initialize(inputManager);
 
    this->rtl = rtl;
    
    sliderBody.setTexture(resourceManager.load<sf::Texture>("ui-slider.png"));
    sliderKnob.setTexture(resourceManager.load<sf::Texture>("ui-slider-knob.png"));
    
    if (rtl) sliderBody.setAnchorPoint(sf::Vector2f(0, sliderBody.getTextureSize().y/2));
    else sliderBody.setAnchorPoint(sf::Vector2f(sliderBody.getTextureSize().x, sliderBody.getTextureSize().y/2));
    sliderKnob.centerAnchorPoint();
    
    slideAxis.registerButton(inputManager, settings.keyboardSettings.moveLeft,  AxisDirection::Negative, 0);
    slideAxis.registerButton(inputManager, settings.keyboardSettings.moveRight, AxisDirection::Positive, 0);
    slideAxis.registerAxis(inputManager, settings.joystickSettings.movementAxisX, 0);
    slideAxis.registerAxis(inputManager, settings.joystickSettings.movementAxisXAlt, 1);
    
    sliderMoveEntry = inputManager.registerMouseMoveCallback([&,this] (sf::Vector2i position)
    {
        mousePosition = position;
    });
}

void UISlider::update()
{
    if (!active || !slideTarget) return;
    if (!parentGroup || this != parentGroup->getCurrentButton()) return;
    
    auto val = rtl ? -slideAxis.getValue() : slideAxis.getValue();
    if (val >= 0.5 && *slideTarget < slideMaximum) slideAction(++(*slideTarget));
    else if (val <= -0.5 && *slideTarget > 0) slideAction(--(*slideTarget));
    
    auto bounds = getBounds();
    auto captionDisplacement = getCaptionDisplacement();
    
    float x;
    if (rtl) x = bounds.left + captionDisplacement.x + sliderKnob.getTextureSize().x/2 +
        sliderBody.getTextureSize().x - (mousePosition.x - getPosition().x);
    else x = mousePosition.x - getPosition().x - (bounds.left + bounds.width - captionDisplacement.x - 
            sliderKnob.getTextureSize().x/2 - sliderBody.getTextureSize().x);
    
    if (state == State::Pressed && x >= -40 && x <= sliderBody.getTextureSize().x + 40)
    {
        float factor = x/sliderBody.getTextureSize().x;
        if (factor < 0) factor = 0; else if (factor > 1) factor = 1;
        *slideTarget = std::roundf(slideMaximum * factor);
        slideAction(*slideTarget);
    }
}

void UISlider::render(Renderer& renderer)
{
    UIButton::render(renderer);
    
    renderer.pushTransform();
    renderer.currentTransform.translate(getPosition());
    
    auto bounds = getBounds();
    auto captionDisplacement = getCaptionDisplacement();
    
    auto displacement = rtl ? bounds.left + captionDisplacement.x + sliderKnob.getTextureSize().x/2 :
        bounds.left + bounds.width - captionDisplacement.x - sliderKnob.getTextureSize().x/2;

    renderer.currentTransform.translate(displacement, 0);
    renderer.pushDrawable(sliderBody, {}, getDepth()+1);
    
    if (slideTarget && slideMaximum)
    {
        auto translation = sliderBody.getTextureSize().x * (1 - float(*slideTarget)/slideMaximum);
        renderer.currentTransform.translate(rtl ? translation : -translation, 0);
        renderer.pushDrawable(sliderKnob, {}, getDepth()+2);
    }
    
    renderer.popTransform();
}
