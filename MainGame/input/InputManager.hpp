#pragma once

#include <SFML/Window.hpp>
#include <non_copyable_movable.hpp>
#include <unordered_map>
#include <functional>
#include <ContainerEntry.hpp>

#include "InputSource.hpp"

class InputManager final : public util::non_copyable_movable
{
public:
    using Callback = std::function<void(InputSource,float)>;

private:
    std::unordered_multimap<InputSource,Callback> callbacks;

public:
    using CallbackEntry = util::ContainerEntry<decltype(callbacks)>;

    InputManager() {}
    ~InputManager() {}

    auto registerCallback(InputSource source, Callback callback)
    {
        return CallbackEntry(callbacks, callbacks.emplace(source, callback));
    }
    
    void dispatchData(InputSource source, float val);

    bool handleEvent(const sf::Event& event);
};

