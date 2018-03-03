#include "InputManager.hpp"

#include <iostream>

void InputManager::dispatchData(InputSource source, float val)
{
    {
        auto it = callbacks.find(source);
        if (it != callbacks.end())
        {
            // Copy required for iterator safety: some callbacks may add others to the list
            auto functions = it->second;
            for (auto &callback : functions) callback.second(source, val);
        }
    }

    for (const auto& src : variableCallbacks)
    {
        if (*src.first != source) continue;
        // ditto
        auto functions = src.second;
        for (auto &callback : functions) callback.second(source, val);
    }
}

void InputManager::dispatchMouseMovement(sf::Vector2i pos)
{
    for (auto& callback : mouseMoveCallbacks) callback.second(pos);
}

bool InputManager::handleEvent(const sf::Event& event)
{
    switch (event.type)
    {
        case sf::Event::KeyPressed:
            dispatchData(InputSource::keyboardKey(event.key.scanCode), 1.0f);
            return true;
        case sf::Event::KeyReleased:
            dispatchData(InputSource::keyboardKey(event.key.scanCode), 0.0f);
            return true;
        case sf::Event::MouseWheelScrolled:   
            dispatchData(InputSource::mouseWheel(event.mouseWheelScroll.wheel), event.mouseWheelScroll.delta);
            return true;
        case sf::Event::MouseButtonPressed:   
            dispatchData(InputSource::mouseButton(event.mouseButton.button), 1.0f);
            return true;
        case sf::Event::MouseButtonReleased:   
            dispatchData(InputSource::mouseButton(event.mouseButton.button), 0.0f);
            return true;
        case sf::Event::MouseMoved:
            dispatchData(InputSource::mouseX, (float)event.mouseMove.x);
            dispatchData(InputSource::mouseY, (float)event.mouseMove.y);
            dispatchMouseMovement(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
            return true;
        case sf::Event::JoystickButtonPressed:
            dispatchData(InputSource::joystickButton(event.joystickButton.button), 1.0f);
            return true;
        case sf::Event::JoystickButtonReleased:
            dispatchData(InputSource::joystickButton(event.joystickButton.button), 0.0f);
            return true;
        case sf::Event::JoystickMoved:
            dispatchData(InputSource::joystickAxis(event.joystickMove.axis), event.joystickMove.position);
            return true;
        default: return false;
    }
}

InputManager::CallbackEntry InputManager::registerCallback(InputSource source, InputManager::Callback callback,
    intmax_t priority)
{
    auto& map = callbacks[source];
    return CallbackEntry(map, map.emplace(priority, callback));
}

InputManager::CallbackEntry InputManager::registerVariableCallback(const InputSource &source,
    InputManager::Callback callback, intmax_t priority)
{
    auto& map = variableCallbacks[&source];
    return CallbackEntry(map, map.emplace(priority, callback));
}

InputManager::MouseMoveEntry InputManager::registerMouseMoveCallback(InputManager::MouseMoveCallback callback,
    intmax_t priority)
{
    return MouseMoveEntry(mouseMoveCallbacks, mouseMoveCallbacks.emplace(priority, callback));
}
