//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#pragma once


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
    using MouseMoveCallback = std::function<void(glm::ivec2)>;

private:
    std::unordered_map<InputSource,PriorityMap> callbacks;
    std::unordered_map<const InputSource*,PriorityMap> variableCallbacks;
    std::multimap<intmax_t,MouseMoveCallback> mouseMoveCallbacks;
    Callback curPickAllKeyboardCallback, curPickAllJoystickCallback;
    enum { None, Positive, Negative } joystickPickAllAxisState;
    bool joystickPickAllAxesOnly, joystickCurrent;

public:
    using CallbackEntry = util::ContainerEntry<PriorityMap>;
    using MouseMoveEntry = util::ContainerEntry<decltype(mouseMoveCallbacks)>;

    InputManager() : joystickPickAllAxisState(None), joystickCurrent(false) {}
    ~InputManager() {}

    CallbackEntry registerCallback(InputSource source, Callback callback, intmax_t priority = 0);
    CallbackEntry registerVariableCallback(const InputSource& source, Callback callback, intmax_t priority = 0);
    MouseMoveEntry registerMouseMoveCallback(MouseMoveCallback callback, intmax_t priority = 0);

    void addPickAllKeyboardCallback(Callback callback);
    void addPickAllJoystickCallback(Callback callback, bool axesOnly = false);
    void handleJoystickPickAllResponseAxis(sf::Joystick::Axis axis, float val);

    void dispatchData(InputSource source, float val);
    void dispatchMouseMovement(glm::ivec2 pos);

    bool handleEvent(const sf::Event& event);
    bool isJoystickCurrent() const { return joystickCurrent; }
};

