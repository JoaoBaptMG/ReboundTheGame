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

#include <utility>
#include "UIButton.hpp"
#include "UIButtonGroup.hpp"

#include "language/LangID.hpp"
#include "input/CommonActions.hpp"
#include "settings/InputSettings.hpp"

#include "Services.hpp"

class UIInputRemapper final : public UIButton
{
public:
    enum class InputDest { Keyboard, JoystickButton, HorJoystickAxis, VertJoystickAxis };

private:
    TextDrawable sourceCaption;
    InputSource& remapSource;
    LocalizationManager& localizationManager;
    InputDest inputDest;

public:
    explicit UIInputRemapper(InputSource& source, LocalizationManager& lm, InputDest inputDest) : UIButton(),
        remapSource(source), localizationManager(lm), inputDest(inputDest) {}
    UIInputRemapper(Services& services, InputSource& source, InputDest inputDest);
    void initialize(Services& services);
    virtual ~UIInputRemapper() {}
    
    auto& getSourceCaption() { return sourceCaption; }
    const auto& getSourceCaption() const { return sourceCaption; }
    
    void resetText();
    void resetRemappingState(InputManager& inputManager);
    
    virtual void render(Renderer &renderer) override;

    friend class UIInputRemappingButtonGroup;
};

void createCommonInputRemapper(UIInputRemapper& button, Services& services,
    std::string activeResourceName, std::string pressedResourceName, sf::FloatRect centerRect,
    sf::FloatRect destRect, LangID captionString, size_t captionSize, sf::Color textColor,
    float outlineThickness, sf::Color outlineColor, sf::Vector2f captionDisplacement);

class UIInputRemappingButtonGroup final : public UIButtonGroup
{
    UIInputRemapper* currentRemappingButton;
    std::function<std::pair<size_t,size_t>(size_t)> groupingFunction;
    InputManager& inputManager;

public:
    UIInputRemappingButtonGroup(Services& services, TravelingMode travelingMode =
        TravelingMode::Vertical);

    void setGroupingFunction(decltype(groupingFunction) func);
    void setRemappingButton(UIInputRemapper* newRemappingButton);
    void assignRemappingUniquely(UIInputRemapper& button, InputSource newSource);
};
