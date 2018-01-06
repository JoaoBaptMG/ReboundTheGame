#include "UIButtonGroup.hpp"

#include "UIButton.hpp"
#include "UIPointer.hpp"

#include <functional>
#include <algorithm>
#include <cmath>

UIButtonGroup::UIButtonGroup(InputManager& inputManager, const InputSettings& settings, TravelingMode travelingMode)
    : currentId(-1), active(true), pointer(nullptr)
{
    select.registerSource(inputManager, settings.keyboardSettings.jumpInput);
    select.registerSource(inputManager, settings.joystickSettings.jumpInput);
    
    if (travelingMode == TravelingMode::Horizontal)
    {
        travel.registerButton(inputManager, settings.keyboardSettings.moveLeft,  AxisDirection::Negative);
        travel.registerButton(inputManager, settings.keyboardSettings.moveRight, AxisDirection::Positive);
        travel.registerAxis(inputManager, settings.joystickSettings.movementAxisX);
    }    
    else
    {
        travel.registerButton(inputManager, settings.keyboardSettings.moveUp,    AxisDirection::Negative);
        travel.registerButton(inputManager, settings.keyboardSettings.moveDown,  AxisDirection::Positive);
        travel.registerAxis(inputManager, settings.joystickSettings.movementAxisY);
    }
    
    select.setAction([=](bool pressed)
    {
        if (!active || buttons.empty()) return;
        if (currentId < buttons.size())
        {
            if (pressed) getCurrentButton()->state = UIButton::State::Pressed;
            else
            {
                getCurrentButton()->state = UIButton::State::Active;
                if (getCurrentButton()->pressAction) getCurrentButton()->pressAction();
            }
            
            if (pointer) pointer->hide();
        }
    });
    
    travel.setAction([=](float val)
    {
        if (!active || (val > -0.5 && val < 0.5) || buttons.size() <= 1) return;
        if (currentId < buttons.size() && getCurrentButton()->state == UIButton::State::Pressed) return;
        
        if (currentId >= buttons.size()) setCurrentId(val >= 0.5 ? 0 : buttons.size()-1);
        else if (val >= 0.5) setCurrentId((currentId + 1) % buttons.size());
        else setCurrentId((currentId + buttons.size() - 1) % buttons.size());
        
        if (pointer) pointer->hide();
    });
}

void UIButtonGroup::setButtons(std::initializer_list<std::reference_wrapper<UIButton>> buttons)
{
    this->buttons.clear();
    for (auto& button : buttons)
    {
        this->buttons.push_back(&button.get());
        button.get().parentGroup = this;
    }
}

void UIButtonGroup::setButtons(UIButton* buttons, size_t size)
{
    this->buttons.clear();
    for (size_t i = 0; i < size; i++)
    {
        this->buttons.push_back(&buttons[i]);
        buttons[i].parentGroup = this;
    }
}

void UIButtonGroup::setCurrentId(size_t id)
{
    if (currentId < buttons.size())
    {
        getCurrentButton()->state = UIButton::State::Normal;
        if (getCurrentButton()->outAction) getCurrentButton()->outAction();
    }
    currentId = id;
    if (currentId < buttons.size())
    {
        getCurrentButton()->state = UIButton::State::Active;
        if (getCurrentButton()->overAction) getCurrentButton()->overAction();
    }
}

void UIButtonGroup::setCurrentButton(UIButton* cur)
{
    setCurrentId(std::find(buttons.begin(), buttons.end(), cur) - buttons.begin());
}
