#pragma once

#include <SFML/Window.hpp>
#include <non_copyable_movable.hpp>
#include <unordered_map>
#include <map>
#include <list>
#include <functional>
#include <vector>
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
    std::unordered_map<const InputSource*,PriorityMap> variableCallbacks;
    std::multimap<intmax_t,MouseMoveCallback> mouseMoveCallbacks;
    Callback curPickAllKeyboardCallback, curPickAllJoystickCallback;

public:
    using CallbackEntry = util::ContainerEntry<PriorityMap>;
    using MouseMoveEntry = util::ContainerEntry<decltype(mouseMoveCallbacks)>;

    InputManager() {}
    ~InputManager() {}

    CallbackEntry registerCallback(InputSource source, Callback callback, intmax_t priority = 0);
    CallbackEntry registerVariableCallback(const InputSource& source, Callback callback, intmax_t priority = 0);
    MouseMoveEntry registerMouseMoveCallback(MouseMoveCallback callback, intmax_t priority = 0);

    void addPickAllKeyboardCallback(Callback callback);
    void addPickAllJoystickCallback(Callback callback);

    void dispatchData(InputSource source, float val);
    void dispatchMouseMovement(sf::Vector2i pos);

    bool handleEvent(const sf::Event& event);
};

