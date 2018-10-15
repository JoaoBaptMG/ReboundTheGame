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


#pragma once

#include "UIButton.hpp"

#include "language/LangID.hpp"
#include "input/CommonActions.hpp"
#include "settings/InputSettings.hpp"

#include "Services.hpp"

class UISlider final : public UIButton
{
    Sprite sliderBody, sliderKnob;
    AxisAction<1,2> slideAxis;
    
    InputManager::MouseMoveEntry sliderMoveEntry;
    
    size_t* slideTarget;
    size_t slideMaximum, soundPlayDelay;
    std::function<void(size_t)> slideAction;
    sf::Vector2i mousePosition;
    float prevX;
    bool rtl;

    Services& services;
    
public:
    UISlider(size_t* target = nullptr, size_t max = 0);
    UISlider(Services& services, size_t* target = nullptr, size_t max = 0);
    void initialize(Services& services);
    virtual ~UISlider() {}
    
    auto getSlideTarget() const { return slideTarget; }
    void setSlideTarget(size_t* target) { slideTarget = target; }
    
    auto getSlideMaximum() const { return slideTarget; }
    void setSlideMaximum(size_t max) { slideMaximum = max; }
    
    auto getSlideAction() const { return slideAction; }
    void setSlideAction(decltype(slideAction) action) { slideAction = action; }

    void updateSoundDelay();
    void resetKnob();
    
    void update();
    virtual void render(Renderer &renderer) override;
};

