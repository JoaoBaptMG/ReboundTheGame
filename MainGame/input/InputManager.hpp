#pragma once

#include <SFML/Window.hpp>
#include <non_copyable_movable.hpp>
#include <unordered_map>
#include <map>
#include <list>
#include <functional>
#include <ContainerEntry.hpp>

#include "InputSource.hpp"

class InputManager final : public util::non_copyable_movable
{
public:
    using Callback = std::function<void(InputSource,float)>;
    using PriorityMap = std::multimap<intmax_t,Callback>;
    using MouseMoveCallback = std::function<void(sf::Vector2i)>;

private:
    std::unordered_map<InputSource,PriorityMap> callbacks;
    std::multimap<intmax_t,MouseMoveCallback> mouseMoveCallbacks;

public:
    using CallbackEntry = util::ContainerEntry<PriorityMap>;
    using MouseMoveEntry = util::ContainerEntry<decltype(mouseMoveCallbacks)>;

    InputManager() {}
    ~InputManager() {}

    auto registerCallback(InputSource source, Callback callback, intmax_t priority = 0)
    {
        auto& map = callbacks[source];
        return CallbackEntry(map, map.emplace(priority, callback));
    }
    
    auto registerMouseMoveCallback(MouseMoveCallback callback, intmax_t priority = 0)
    {
        return MouseMoveEntry(mouseMoveCallbacks, mouseMoveCallbacks.emplace(priority, callback));
    }
    
    void dispatchData(InputSource source, float val);
    void dispatchMouseMovement(sf::Vector2i pos);

    bool handleEvent(const sf::Event& event);
};

