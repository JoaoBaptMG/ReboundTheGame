#pragma once

#include <vector>
#include <memory>
#include <initializer_list>

#include "input/CommonActions.hpp"
#include "settings/Settings.hpp"

class UIButton;
class UIPointer;

enum class TravelingMode { Horizontal, Vertical };

class UIButtonGroup
{
    std::vector<UIButton*> buttons;
    UIPointer* pointer;
    size_t currentId;
    bool active;

    ButtonAction<2> select;
    AxisAction<1> travel;
    
public:
    UIButtonGroup(InputManager& inputManager, const InputSettings& settings, TravelingMode travelingMode =
        TravelingMode::Vertical);
        
    void setButtons(std::initializer_list<std::reference_wrapper<UIButton>> buttons);
    void setButtons(UIButton* buttons, size_t size);
    
    template <size_t N>
    void setButtons(UIButton (&buttons)[N]) { setButtons(buttons, N); }
    
    void setButtons(std::vector<UIButton>& buttons) { setButtons(buttons.data(), buttons.size()); }
    void setButtons(const std::vector<UIButton*>& buttons);

    auto& getButtons() { return buttons; }
    const auto& getButtons() const { return buttons; }
    
    auto getCurrentId() const { return currentId; }
    auto getCurrentButton() const { return currentId >= buttons.size() ? nullptr : buttons.at(currentId); }
    void setCurrentId(size_t id);
    void setCurrentButton(UIButton* cur);
    
    auto getPointer() const { return pointer; }
    void setPointer(UIPointer& ptr) { pointer = &ptr; }
    
    void activate() { active = true; }
    void deactivate() { active = false; }
};
