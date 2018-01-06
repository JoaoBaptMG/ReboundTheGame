#include "CommonActions.hpp"
#include <iostream>

void ButtonAction::registerSource(InputManager& inputManager, InputSource source)
{
    callbackEntries.push_back(inputManager.registerCallback(source, [=](InputSource source, float val)
    {
        cur = val > 0.5f;
        if (action) action(cur);
    }));
}

void ButtonAction::update()
{
    last2 = last1; last1 = cur;
}

void AxisAction::registerAxis(InputManager& inputManager, InputSource source)
{
    callbackEntries.push_back(inputManager.registerCallback(source, [=](InputSource source, float val)
    {
        valuePos = val;
        valueNeg = 0;
        if (action) action(valuePos - valueNeg);
    }));
}

void AxisAction::registerButton(InputManager& inputManager, InputSource source, AxisDirection dir)
{
    callbackEntries.push_back(inputManager.registerCallback(source, [=](InputSource source, float val)
    {
        if (dir == AxisDirection::Positive) valuePos = val; else valueNeg = val;
        if (action) action(valuePos - valueNeg);
    }));
}

void DualAxisAction::registerAxisForX(InputManager& inputManager, InputSource source)
{
    callbackEntries.push_back(inputManager.registerCallback(source, [=](InputSource source, float val)
    {
        valuePos.x = val;
        valueNeg.x = 0;
        if (action) action(valuePos - valueNeg);
    }));
}

void DualAxisAction::registerAxisForY(InputManager& inputManager, InputSource source)
{
    callbackEntries.push_back(inputManager.registerCallback(source, [=](InputSource source, float val)
    {
        valuePos.y = val;
        valueNeg.y = 0;
        if (action) action(valuePos - valueNeg);
    }));
}

void DualAxisAction::registerButtonForX(InputManager& inputManager, InputSource source, AxisDirection dir)
{
    callbackEntries.push_back(inputManager.registerCallback(source, [=](InputSource source, float val)
    {
        if (dir == AxisDirection::Positive) valuePos.x = val; else valueNeg.x = val;
        if (action) action(valuePos - valueNeg);
    }));
}

void DualAxisAction::registerButtonForY(InputManager& inputManager, InputSource source, AxisDirection dir)
{
    callbackEntries.push_back(inputManager.registerCallback(source, [=](InputSource source, float val)
    {
        if (dir == AxisDirection::Positive) valuePos.y = val; else valueNeg.y = val;
        if (action) action(valuePos - valueNeg);
    }));
}
