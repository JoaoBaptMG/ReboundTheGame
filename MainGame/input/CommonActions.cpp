#include "CommonActions.hpp"

void ButtonAction::registerSource(InputManager& inputManager, InputSource source)
{
    callbackEntries.push_back(inputManager.registerCallback(source, [=](InputSource source, float val)
    {
        cur = val > 0.5f;
    }));
}

void ButtonAction::update()
{
    last2 = last1; last1 = cur;
}

void DualAxisAction::registerAxisForX(InputManager& inputManager, InputSource source)
{
    callbackEntries.push_back(inputManager.registerCallback(source, [=](InputSource source, float val)
    {
        valuePos.x = val;
        valueNeg.x = 0;
    }));
}

void DualAxisAction::registerAxisForY(InputManager& inputManager, InputSource source)
{
    callbackEntries.push_back(inputManager.registerCallback(source, [=](InputSource source, float val)
    {
        valuePos.y = val;
        valueNeg.y = 0;
    }));
}

void DualAxisAction::registerButtonForX(InputManager& inputManager, InputSource source, AxisDirection dir)
{
    callbackEntries.push_back(inputManager.registerCallback(source, [=](InputSource source, float val)
    {
        if (dir == AxisDirection::Positive) valuePos.x = val; else valueNeg.x = val;
    }));
}

void DualAxisAction::registerButtonForY(InputManager& inputManager, InputSource source, AxisDirection dir)
{
    callbackEntries.push_back(inputManager.registerCallback(source, [=](InputSource source, float val)
    {
        if (dir == AxisDirection::Positive) valuePos.y = val; else valueNeg.y = val;
    }));
}
