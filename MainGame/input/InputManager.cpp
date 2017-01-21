#include "InputManager.hpp"

InputManager::InputManager()
{
}

InputManager::~InputManager()
{
}

void InputManager::handleKeyEvent(sf::Keyboard::Key key, ActionState state)
{
    auto mapIt = keyMappings.find(key);

    if (mapIt != keyMappings.end())
    {
        auto itPair = mapCallbacks.equal_range(mapIt->second);
        for (auto it = itPair.first; it != itPair.second; ++it)
            it->second(state);
    }

    auto itPair = rawCallbacks.equal_range(key);
    for (auto it = itPair.first; it != itPair.second; ++it)
        it->second(state);
}

void InputManager::setMapping(sf::Keyboard::Key key, Action action)
{
    keyMappings[key] = action;
}

InputManager::CallbackEntry InputManager::registerCallback(Action action, InputManager::CallbackFunction callback)
{
    return CallbackEntry(mapCallbacks, mapCallbacks.emplace(action, callback));
}

InputManager::RawCallbackEntry InputManager::registerRawCallback(
    sf::Keyboard::Key key, InputManager::CallbackFunction callback)
{
    return RawCallbackEntry(rawCallbacks, rawCallbacks.emplace(key, callback));
}
