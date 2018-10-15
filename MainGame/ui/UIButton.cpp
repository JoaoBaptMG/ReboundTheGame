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
#include <cmath>

UIButton::UIButton(InputManager& inputManager, intmax_t priority) : UIButton()
{
    initialize(inputManager, priority);
}

void UIButton::initialize(InputManager& inputManager, intmax_t priority)
{
    mouseMovedEntry = inputManager.registerMouseMoveCallback([=] (glm::ivec2 position)
    {
        if (!active) return;

        if (state == State::Normal && isInBounds(glm::vec2(position)))
        {
            if (parentGroup) parentGroup->setCurrentButton(this);
            else
            {
                state = State::Active;
                if (overAction) overAction();
            }
        }
        else if (state != State::Normal && !isInBounds(glm::vec2(position)))
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

bool UIButton::isInBounds(glm::vec2 position) const
{
    if (!std::isnan(globalBounds.x) && !std::isnan(globalBounds.y))
        if (!globalBounds.contains(position)) return false;
    return bounds.contains(position - this->position);
}

void UIButton::autoComputeBounds()
{
    bounds = util::rect(0, 0, 0, 0);
    
    if (normalSprite) bounds = bounds.unite(normalSprite->getBounds());
    if (activeSprite) bounds = bounds.unite(activeSprite->getBounds());
    if (pressedSprite) bounds = bounds.unite(pressedSprite->getBounds());
}

void UIButton::render(Renderer &renderer)
{
    renderer.pushTransform();
    renderer.currentTransform *= util::translate(position);
    
    if (state == State::Normal && normalSprite) renderer.pushDrawable(*normalSprite, {}, depth);
    if (state == State::Active && activeSprite) renderer.pushDrawable(*activeSprite, {}, depth);
    if (state == State::Pressed && pressedSprite) renderer.pushDrawable(*pressedSprite, {}, depth);
    
    if (caption)
    {
        renderer.pushTransform();
        
        switch (caption->getHorizontalAnchor())
        {
            case TextDrawable::HorAnchor::Left: 
                renderer.currentTransform *= util::translate(floorf(bounds.x + captionDisplacement.x), 0); break;
            case TextDrawable::HorAnchor::Center: 
                renderer.currentTransform *= util::translate(floorf(bounds.x + bounds.width/2), 0); break;
            case TextDrawable::HorAnchor::Right: 
                renderer.currentTransform *= util::translate(floorf(bounds.x + bounds.width - captionDisplacement.x), 0); break;
        }
        
        switch (caption->getVerticalAnchor())
        {
            case TextDrawable::VertAnchor::Top: 
                renderer.currentTransform *= util::translate(0, floorf(bounds.y + captionDisplacement.y)); break;
            case TextDrawable::VertAnchor::Center: 
                renderer.currentTransform *= util::translate(0, floorf(bounds.y + bounds.height/2)); break;
            case TextDrawable::VertAnchor::Bottom: 
            case TextDrawable::VertAnchor::Baseline:
                renderer.currentTransform *= util::translate(0, floorf(bounds.y + bounds.height - captionDisplacement.y)); break;
        }
        
        renderer.pushDrawable(*caption, {}, depth);
        renderer.popTransform();
    }
    
    renderer.popTransform();
}
