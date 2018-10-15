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

#include "InputManager.hpp"

#include "preventSleeping.hpp"
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
            if (curPickAllKeyboardCallback)
                curPickAllKeyboardCallback(InputSource::keyboardKey(event.key.scanCode), 1.0f);
            else dispatchData(InputSource::keyboardKey(event.key.scanCode), 1.0f);
            joystickCurrent = false;
            return true;
        case sf::Event::KeyReleased:
            if (curPickAllKeyboardCallback)
            {
                curPickAllKeyboardCallback(InputSource::keyboardKey(event.key.scanCode), 0.0f);
                curPickAllKeyboardCallback = Callback();
            }
            else dispatchData(InputSource::keyboardKey(event.key.scanCode), 0.0f);
            joystickCurrent = false;
            return true;
        case sf::Event::MouseWheelScrolled:   
            dispatchData(InputSource::mouseWheel(event.mouseWheelScroll.wheel), event.mouseWheelScroll.delta);
            dispatchData(InputSource::invertedMouseWheel(event.mouseWheelScroll.wheel), -event.mouseWheelScroll.delta);
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
            dispatchData(InputSource::invertedMouseX, -(float)event.mouseMove.x);
            dispatchData(InputSource::invertedMouseY, -(float)event.mouseMove.y);
            dispatchMouseMovement(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
            return true;
        case sf::Event::JoystickButtonPressed:
            if (curPickAllJoystickCallback && !joystickPickAllAxesOnly)
                curPickAllJoystickCallback(InputSource::joystickButton(event.joystickButton.button), 1.0f);
            else dispatchData(InputSource::joystickButton(event.joystickButton.button), 1.0f);
            preventComputerFromSleeping();
            joystickCurrent = true;
            return true;
        case sf::Event::JoystickButtonReleased:
            if (curPickAllJoystickCallback && !joystickPickAllAxesOnly)
            {
                curPickAllJoystickCallback(InputSource::joystickButton(event.joystickButton.button), 0.0f);
                curPickAllJoystickCallback = Callback();
            }
            else dispatchData(InputSource::joystickButton(event.joystickButton.button), 0.0f);
            preventComputerFromSleeping();
            joystickCurrent = true;
            return true;
        case sf::Event::JoystickMoved:
            if (curPickAllJoystickCallback)
                handleJoystickPickAllResponseAxis(event.joystickMove.axis, event.joystickMove.position);
            else
            {
                dispatchData(InputSource::joystickAxis(event.joystickMove.axis), event.joystickMove.position/100.0f);
                dispatchData(InputSource::invertedJoystickAxis(event.joystickMove.axis), -event.joystickMove.position/100.0f);
            }
            preventComputerFromSleeping();
            joystickCurrent = true;
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

void InputManager::addPickAllKeyboardCallback(InputManager::Callback callback)
{
    curPickAllKeyboardCallback = callback;
}

void InputManager::addPickAllJoystickCallback(InputManager::Callback callback, bool axesOnly)
{
    curPickAllJoystickCallback = callback;
    joystickPickAllAxesOnly = axesOnly;
}

void InputManager::handleJoystickPickAllResponseAxis(sf::Joystick::Axis axis, float val)
{
    if (joystickPickAllAxisState == Positive && val < 50.0f)
    {
        curPickAllJoystickCallback(InputSource::joystickAxis(axis), 0.0f);
        curPickAllJoystickCallback = Callback();
        joystickPickAllAxisState = None;
    }
    else if (joystickPickAllAxisState == Negative && val > -50.0f)
    {
        curPickAllJoystickCallback(InputSource::invertedJoystickAxis(axis), 0.0f);
        curPickAllJoystickCallback = Callback();
        joystickPickAllAxisState = None;
    }
    else if (joystickPickAllAxisState == None)
    {
        if (val >= 50.0f)
        {
            curPickAllJoystickCallback(InputSource::joystickAxis(axis), 1.0f);
            joystickPickAllAxisState = Positive;
        }
        else if (val <= -50.0f)
        {
            curPickAllJoystickCallback(InputSource::invertedJoystickAxis(axis), 1.0f);
            joystickPickAllAxisState = Negative;
        }
    }
}
