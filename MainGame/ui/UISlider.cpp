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
    size_t* target, size_t max) : UISlider(target, max)
{
    initialize(inputManager, resourceManager, settings);
}

void UISlider::initialize(InputManager& inputManager, ResourceManager& resourceManager, const InputSettings& settings)
{
    UIButton::initialize(inputManager);
    
    sliderBody.setTexture(resourceManager.load<sf::Texture>("ui-slider.png"));
    sliderKnob.setTexture(resourceManager.load<sf::Texture>("ui-slider-knob.png"));
    
    sliderBody.setAnchorPoint(sf::Vector2f(sliderBody.getTextureSize().x, sliderBody.getTextureSize().y/2));
    sliderKnob.centerAnchorPoint();
    
    slideAxis.registerButton(inputManager, settings.keyboardSettings.moveLeft,  AxisDirection::Negative);
    slideAxis.registerButton(inputManager, settings.keyboardSettings.moveRight, AxisDirection::Positive);
    slideAxis.registerAxis(inputManager, settings.joystickSettings.movementAxisX);
    
    sliderMoveEntry = inputManager.registerMouseMoveCallback([&,this] (sf::Vector2i position)
    {
        mousePosition = position;
    });
}

void UISlider::update()
{
    if (!active || !slideTarget) return;
    if (!parentGroup || this != parentGroup->getCurrentButton()) return;
    if (slideAxis.getValue() >= 0.5 && *slideTarget < slideMaximum) slideAction(++(*slideTarget));
    else if (slideAxis.getValue() <= -0.5 && *slideTarget > 0) slideAction(--(*slideTarget));
    
    auto bounds = getBounds();
    auto captionDisplacement = getCaptionDisplacement();
    auto displacement = bounds.left + bounds.width - captionDisplacement.x - sliderKnob.getTextureSize().x/2;
    auto x = mousePosition.x - getPosition().x - displacement;
    if (state == State::Pressed && x >= -(float)sliderBody.getTextureSize().x - 40 && x <= 40)
    {
        float factor = 1 + x/sliderBody.getTextureSize().x;
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
    
    auto displacement = bounds.left + bounds.width - captionDisplacement.x - sliderKnob.getTextureSize().x/2;
    renderer.currentTransform.translate(displacement, 0);
    renderer.pushDrawable(sliderBody, {}, getDepth()+1);
    
    if (slideTarget && slideMaximum)
    {
        auto translation = sliderBody.getTextureSize().x * (1 - float(*slideTarget)/slideMaximum);
        renderer.currentTransform.translate(-translation, 0);
        renderer.pushDrawable(sliderKnob, {}, getDepth()+2);
    }
    
    renderer.popTransform();
}
