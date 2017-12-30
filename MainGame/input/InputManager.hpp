#pragma once

#include <SFML/Window.hpp>
#include <non_copyable_movable.hpp>
#include <unordered_map>
#include <list>
#include <functional>
#include <ContainerEntry.hpp>

#include "InputSource.hpp"

class InputManager final : public util::non_copyable_movable
{
public:
    using Callback = std::function<void(InputSource,float)>;
    using MouseMoveCallback = std::function<void(sf::Vector2i)>;

private:
    std::unordered_multimap<InputSource,Callback> callbacks;
    std::list<MouseMoveCallback> mouseMoveCallbacks;

public:
    using CallbackEntry = util::ContainerEntry<decltype(callbacks)>;
    using MouseMoveEntry = util::ContainerEntry<decltype(mouseMoveCallbacks)>;

    InputManager() {}
    ~InputManager() {}

    auto registerCallback(InputSource source, Callback callback)
    {
        return CallbackEntry(callbacks, callbacks.emplace(source, callback));
    }
    
    auto registerMouseMoveCallback(MouseMoveCallback callback)
    {
        auto it = mouseMoveCallbacks.emplace(mouseMoveCallbacks.end(), callback);
        return MouseMoveEntry(mouseMoveCallbacks, it);
    }
    
    void dispatchData(InputSource source, float val);
    void dispatchMouseMovement(sf::Vector2i pos);

    bool handleEvent(const sf::Event& event);
};

