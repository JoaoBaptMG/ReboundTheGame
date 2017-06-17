#pragma once

#include "InputManager.hpp"
#include <SFML/System.hpp>
#include <vector>

class ButtonAction final
{
    std::vector<InputManager::CallbackEntry> callbackEntries;
    bool cur = false, last1 = false, last2 = false;

    const bool& active;

public:
    ButtonAction(const bool& activationTrigger) : active(activationTrigger) {}
    ~ButtonAction() {}

    void registerSource(InputManager& inputManager, InputSource source);
    void update();

    bool isTriggered() const { return active && last1 && !last2; }
    bool isPressed() const { return active && last1; }
    bool isReleased() const { return active && !last1 && last2; }
};

enum class AxisDirection { Positive, Negative };

class DualAxisAction final
{
    std::vector<InputManager::CallbackEntry> callbackEntries;
    sf::Vector2f valuePos;
    sf::Vector2f valueNeg;

    const bool& active;

public:
    DualAxisAction(const bool& activationTrigger) : active(activationTrigger) {}
    ~DualAxisAction() {}

    void registerAxisForX(InputManager& inputManager, InputSource source);
    void registerAxisForY(InputManager& inputManager, InputSource source);

    void registerButtonForX(InputManager& inputManager, InputSource source, AxisDirection dir);
    void registerButtonForY(InputManager& inputManager, InputSource source, AxisDirection dir);

    sf::Vector2f getValue() const
    {
        return active ? valuePos - valueNeg : sf::Vector2f(0, 0);
    }
};
