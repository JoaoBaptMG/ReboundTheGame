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
#include "rendering/Texture.hpp"

#include <cmath>
#include <algorithm>

#include "resources/ResourceManager.hpp"
#include "language/LocalizationManager.hpp"
#include "language/convenienceConfigText.hpp"


constexpr size_t MaxDelay = 2;

UISlider::UISlider(Services& services, size_t* target, size_t max) : UIButton(),
    slideTarget(target), slideMaximum(max), rtl(false), soundPlayDelay(0), prevX(0), services(services)
{
    initialize(services);
}

void UISlider::initialize(Services& services)
{
    UIButton::initialize(services.inputManager);
 
    rtl = services.localizationManager.isRTL();
    
    sliderBody.setTexture(services.resourceManager.load<Texture>("ui-slider.png"));
    sliderKnob.setTexture(services.resourceManager.load<Texture>("ui-slider-knob.png"));
    
    if (rtl) sliderBody.setAnchorPoint(glm::vec2(0, sliderBody.getTextureSize().y/2));
    else sliderBody.setAnchorPoint(glm::vec2(sliderBody.getTextureSize().x, sliderBody.getTextureSize().y/2));
    sliderKnob.centerAnchorPoint();

    auto& settings = services.settings.inputSettings;
    slideAxis.registerButton(services.inputManager, settings.keyboardSettings.moveLeft,  AxisDirection::Negative, 0);
    slideAxis.registerButton(services.inputManager, settings.keyboardSettings.moveRight, AxisDirection::Positive, 0);
    slideAxis.registerAxis(services.inputManager, settings.joystickSettings.movementAxisX, 0);
    slideAxis.registerAxis(services.inputManager, settings.joystickSettings.movementAxisXAlt, 1);
    
    sliderMoveEntry = services.inputManager.registerMouseMoveCallback([&,this] (glm::ivec2 position)
    {
        mousePosition = position;
    });
}

void UISlider::update()
{
    if (!active || !slideTarget) return;
    if (!parentGroup || this != parentGroup->getCurrentButton()) return;
    
    auto val = rtl ? -slideAxis.getValue() : slideAxis.getValue();
    if (val >= 0.5 && *slideTarget < slideMaximum)
    {
        updateSoundDelay();
        slideAction(++(*slideTarget));
    }
    else if (val <= -0.5 && *slideTarget > 0)
    {
        updateSoundDelay();
        slideAction(--(*slideTarget));
    }
    
    auto bounds = getBounds();
    auto captionDisplacement = getCaptionDisplacement();
    
    float x;
    if (rtl) x = bounds.x + captionDisplacement.x + sliderKnob.getTextureSize().x/2 +
        sliderBody.getTextureSize().x - (mousePosition.x - getPosition().x);
    else x = mousePosition.x - getPosition().x - (bounds.x + bounds.width - captionDisplacement.x - 
            sliderKnob.getTextureSize().x/2 - sliderBody.getTextureSize().x);
    
    if (state == State::Pressed && x >= -40 && x <= sliderBody.getTextureSize().x + 40)
    {
        if (prevX != x) updateSoundDelay();

        float factor = x/sliderBody.getTextureSize().x;
        if (factor < 0) factor = 0; else if (factor > 1) factor = 1;
        *slideTarget = std::roundf(slideMaximum * factor);
        slideAction(*slideTarget);
    }

    prevX = x;
}

void UISlider::updateSoundDelay()
{
    soundPlayDelay++;
    if (soundPlayDelay >= MaxDelay)
    {
        soundPlayDelay = 0;
        playCursor(services);
    }
}

void UISlider::render(Renderer& renderer)
{
    UIButton::render(renderer);
    
    renderer.pushTransform();
    renderer.currentTransform *= util::translate(getPosition());
    
    auto bounds = getBounds();
    auto captionDisplacement = getCaptionDisplacement();
    
    auto displacement = rtl ? bounds.x + captionDisplacement.x + sliderKnob.getTextureSize().x/2 :
        bounds.x + bounds.width - captionDisplacement.x - sliderKnob.getTextureSize().x/2;

    renderer.currentTransform *= util::translate(displacement, 0);
    renderer.pushDrawable(sliderBody, {}, getDepth()+1);
    
    if (slideTarget && slideMaximum)
    {
        auto translation = sliderBody.getTextureSize().x * (1 - float(*slideTarget)/slideMaximum);
        renderer.currentTransform *= util::translate(rtl ? translation : -translation, 0);
        renderer.pushDrawable(sliderKnob, {}, getDepth()+2);
    }
    
    renderer.popTransform();
}
