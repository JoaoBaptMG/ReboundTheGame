#pragma once

#include "InputManager.hpp"
#include <SFML/System.hpp>
#include <vector>

class ButtonAction final
{
    std::vector<InputManager::CallbackEntry> callbackEntries;
    bool cur = false, last1 = false, last2 = false;
    
    std::function<void(bool)> action;

public:
    ButtonAction() {}
    ~ButtonAction() {}

    void registerSource(InputManager& inputManager, InputSource source);
    void update();

    bool isTriggered() const { return last1 && !last2; }
    bool isPressed() const { return last1; }
    bool isReleased() const { return !last1 && last2; }
    
    auto getAction() const { return action; }
    void setAction(decltype(action) action) { this->action = action; }
};

enum class AxisDirection { Positive, Negative };

class AxisAction final
{
    std::vector<InputManager::CallbackEntry> callbackEntries;
    float valuePos;
    float valueNeg;

    std::function<void(float)> action;

public:
    AxisAction() : valuePos(), valueNeg() {}
    ~AxisAction() {}

    void registerAxis(InputManager& inputManager, InputSource source);
    void registerButton(InputManager& inputManager, InputSource source, AxisDirection dir);

    float getValue() const
    {
        return valuePos - valueNeg;
    }
    
    auto getAction() const { return action; }
    void setAction(decltype(action) action) { this->action = action; }
};

class DualAxisAction final
{
    std::vector<InputManager::CallbackEntry> callbackEntries;
    sf::Vector2f valuePos;
    sf::Vector2f valueNeg;

    std::function<void(sf::Vector2f)> action;

public:
    DualAxisAction() : valuePos(), valueNeg() {}
    ~DualAxisAction() {}

    void registerAxisForX(InputManager& inputManager, InputSource source);
    void registerAxisForY(InputManager& inputManager, InputSource source);

    void registerButtonForX(InputManager& inputManager, InputSource source, AxisDirection dir);
    void registerButtonForY(InputManager& inputManager, InputSource source, AxisDirection dir);

    sf::Vector2f getValue() const
    {
        return valuePos - valueNeg;
    }
    
    auto getAction() const { return action; }
    void setAction(decltype(action) action) { this->action = action; }
};
