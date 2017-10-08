#include "InputManager.hpp"

void InputManager::dispatchData(InputSource source, float val)
{
    auto iters = callbacks.equal_range(source);
    for (auto it = iters.first; it != iters.second; ++it)
        it->second(source, val);
}

bool InputManager::handleEvent(const sf::Event& event)
{
    switch (event.type)
    {
        case sf::Event::KeyPressed:
            dispatchData(InputSource::keyboardKey(event.key.code), 1.0f);
            return true;
        case sf::Event::KeyReleased:
            dispatchData(InputSource::keyboardKey(event.key.code), 0.0f);
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
            return true;
        case sf::Event::JoystickButtonPressed:
            dispatchData(InputSource::joystickButton(event.joystickButton.joystickId,
                         event.joystickButton.button), 1.0f);
            return true;
        case sf::Event::JoystickButtonReleased:
            dispatchData(InputSource::joystickButton(event.joystickButton.joystickId,
                         event.joystickButton.button), 0.0f);
            return true;
        case sf::Event::JoystickMoved:
            dispatchData(InputSource::joystickAxis(event.joystickMove.joystickId,
                         event.joystickMove.axis), event.joystickMove.position);
            return true;
        default: return false;
    }
}
