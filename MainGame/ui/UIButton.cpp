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

#include "UIButton.hpp"

#include "UIButtonGroup.hpp"

#include "rendering/Renderer.hpp"
#include <rectUtils.hpp>
#include <cmath>

UIButton::UIButton(InputManager& inputManager, intmax_t priority) : UIButton()
{
    initialize(inputManager, priority);
}

void UIButton::initialize(InputManager& inputManager, intmax_t priority)
{
    mouseMovedEntry = inputManager.registerMouseMoveCallback([=] (sf::Vector2i position)
    {
        if (!active) return;

        if (state == State::Normal && isInBounds(sf::Vector2f(position)))
        {
            if (parentGroup) parentGroup->setCurrentButton(this);
            else
            {
                state = State::Active;
                if (overAction) overAction();
            }
        }
        else if (state != State::Normal && !isInBounds(sf::Vector2f(position)))
        {
            if (parentGroup) parentGroup->setCurrentButton(nullptr);
            else
            {
                state = State::Normal;
                if (outAction) outAction();
            }
        }
    });
    
    mouseButtonEntry = inputManager.registerCallback(InputSource::mouseButton(sf::Mouse::Button::Left),
    [=] (InputSource, float val)
    {
        if (!active) return;
        
        if (val > 0.5)
        {
            if (state == State::Active)
                state = State::Pressed;
        }
        else
        {
            if (state == State::Pressed)
            {
                state = State::Active;
                if (pressAction) pressAction();
            }
        }
    }, priority);
}

bool UIButton::isInBounds(sf::Vector2f position) const
{
    if (!std::isnan(globalBounds.left) && !std::isnan(globalBounds.top))
        if (!globalBounds.contains(position)) return false;
    return bounds.contains(position - this->position);
}

void UIButton::autoComputeBounds()
{
    bounds = sf::FloatRect(0, 0, 0, 0);
    
    if (normalSprite) bounds = rectUnionWithRect(bounds, normalSprite->getBounds());
    if (activeSprite) bounds = rectUnionWithRect(bounds, activeSprite->getBounds());
    if (pressedSprite) bounds = rectUnionWithRect(bounds, pressedSprite->getBounds());
}

void UIButton::render(Renderer &renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(position);
    
    if (state == State::Normal && normalSprite) renderer.pushDrawable(*normalSprite, {}, depth);
    if (state == State::Active && activeSprite) renderer.pushDrawable(*activeSprite, {}, depth);
    if (state == State::Pressed && pressedSprite) renderer.pushDrawable(*pressedSprite, {}, depth);
    
    if (caption)
    {
        renderer.pushTransform();
        
        switch (caption->getHorizontalAnchor())
        {
            case TextDrawable::HorAnchor::Left: 
                renderer.currentTransform.translate(floorf(bounds.left + captionDisplacement.x), 0); break;
            case TextDrawable::HorAnchor::Center: 
                renderer.currentTransform.translate(floorf(bounds.left + bounds.width/2), 0); break;
            case TextDrawable::HorAnchor::Right: 
                renderer.currentTransform.translate(floorf(bounds.left + bounds.width - captionDisplacement.x), 0); break;
        }
        
        switch (caption->getVerticalAnchor())
        {
            case TextDrawable::VertAnchor::Top: 
                renderer.currentTransform.translate(0, floorf(bounds.top + captionDisplacement.y)); break;
            case TextDrawable::VertAnchor::Center: 
                renderer.currentTransform.translate(0, floorf(bounds.top + bounds.height/2)); break;
            case TextDrawable::VertAnchor::Bottom: 
            case TextDrawable::VertAnchor::Baseline:
                renderer.currentTransform.translate(0, floorf(bounds.top + bounds.height - captionDisplacement.y)); break;
        }
        
        renderer.pushDrawable(*caption, {}, depth);
        renderer.popTransform();
    }
    
    renderer.popTransform();
}
