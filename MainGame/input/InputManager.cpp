#include "InputManager.hpp"

#include <iostream>

void InputManager::dispatchData(InputSource source, float val)
{
    auto iters = callbacks.equal_range(source);
    
    // This is needed as the callbacks may push/pop other callbacks into the stack, invalidating the iterators
    std::vector<decltype(callbacks)::mapped_type> functions;
    for (auto it = iters.first; it != iters.second; ++it)
        functions.push_back(it->second);
        
    for (auto& callback : functions) callback(source, val);
}

void InputManager::dispatchMouseMovement(sf::Vector2i pos)
{
    for (auto& callback : mouseMoveCallbacks) callback(pos);
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
