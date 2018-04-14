#pragma once

#include "InputManager.hpp"
#include "gameplay/VirtualActions.hpp"
#include <SFML/System.hpp>
#include <vector>

class ButtonActionBase : public VirtualButtonAction
{
protected:
    bool last1 = false, last2 = false;
    std::function<void(bool)> action;
    bool cur = false;

    ButtonActionBase() {}
    ~ButtonActionBase() {}

public:
    void update();

    virtual bool isTriggered() const override { return last1 && !last2; }
    virtual bool isPressed() const override { return last1; }
    virtual bool isReleased() const override { return !last1 && last2; }
    
    auto getAction() const { return action; }
    void setAction(decltype(action) action) { this->action = action; }
};

template <size_t N>
class ButtonAction final : public ButtonActionBase
{
    InputManager::CallbackEntry callbackEntries[N];

public:
    ButtonAction() {}
    ~ButtonAction() {}

    void registerSource(InputManager& inputManager, const InputSource& source, size_t n)
    {
        if (n < N) callbackEntries[n] = inputManager.registerVariableCallback(source, [=](InputSource source, float val)
            {
                cur = val > 0.5f;
                if (action) action(cur);
            });
    }
};

enum class AxisDirection { Positive, Negative };

class AxisActionBase : public VirtualAxisAction
{
protected:
    std::function<void(float)> action;
    float valuePos;
    float valueNeg;

    AxisActionBase() : valuePos(), valueNeg() {}
    ~AxisActionBase() {}

public:
    virtual float getValue() const override
    {
        return valuePos - valueNeg;
    }
    
    auto getAction() const { return action; }
    void setAction(decltype(action) action) { this->action = action; }
};

template <size_t ButN, size_t AxisN>
class AxisAction final : public AxisActionBase
{
    InputManager::CallbackEntry buttonCallbackEntries[2*ButN];
    InputManager::CallbackEntry axisCallbackEntries[AxisN];

public:
    AxisAction() {}
    ~AxisAction() {}

    void registerButton(InputManager& inputManager, const InputSource& source, AxisDirection dir, size_t n)
    {
        if (n < ButN) buttonCallbackEntries[2*n + (dir == AxisDirection::Positive)] =
            inputManager.registerVariableCallback(source, [=](InputSource source, float val)
            {
                if (dir == AxisDirection::Positive) valuePos = val; else valueNeg = val;
                if (action) action(valuePos - valueNeg);
            });
    }

    void registerAxis(InputManager& inputManager, const InputSource& source, size_t n)
    {
        if (n < AxisN) axisCallbackEntries[n] = inputManager.registerVariableCallback(source, [=](InputSource source, float val)
        {
            valuePos = val;
            valueNeg = 0;
            if (action) action(valuePos - valueNeg);
        });
    }
};

class DualAxisActionBase : public VirtualDualAxisAction
{
protected:
    sf::Vector2f valuePos;
    sf::Vector2f valueNeg;

    std::function<void(sf::Vector2f)> action;

    DualAxisActionBase() : valuePos(), valueNeg() {}
    ~DualAxisActionBase() {}

public:
    virtual sf::Vector2f getValue() const override
    {
        return valuePos - valueNeg;
    }
    
    auto getAction() const { return action; }
    void setAction(decltype(action) action) { this->action = action; }
};

template <size_t ButN, size_t AxisN>
class DualAxisAction final : public DualAxisActionBase
{
    InputManager::CallbackEntry buttonCallbackEntriesX[2*ButN];
    InputManager::CallbackEntry axisCallbackEntriesX[AxisN];
    InputManager::CallbackEntry buttonCallbackEntriesY[2*ButN];
    InputManager::CallbackEntry axisCallbackEntriesY[AxisN];

public:
    void registerButtonForX(InputManager& inputManager, const InputSource& source, AxisDirection dir, size_t n)
    {
        if (n < ButN) buttonCallbackEntriesX[2*n + (dir == AxisDirection::Positive)] =
            inputManager.registerVariableCallback(source, [=](InputSource source, float val)
            {
                if (dir == AxisDirection::Positive) valuePos.x = val; else valueNeg.x = val;
                if (action) action(valuePos - valueNeg);
            });
    }

    void registerButtonForY(InputManager& inputManager, const InputSource& source, AxisDirection dir, size_t n)
    {
        if (n < ButN) buttonCallbackEntriesY[2*n + (dir == AxisDirection::Positive)] =
            inputManager.registerVariableCallback(source, [=](InputSource source, float val)
            {
                if (dir == AxisDirection::Positive) valuePos.y = val; else valueNeg.y = val;
                if (action) action(valuePos - valueNeg);
            });
    }

    void registerAxisForX(InputManager& inputManager, const InputSource& source, size_t n)
    {
        if (n < AxisN) axisCallbackEntriesX[n] = inputManager.registerVariableCallback(source, [=](InputSource source, float val)
        {
            valuePos.x = val;
            valueNeg.x = 0;
            if (action) action(valuePos - valueNeg);
        });
    }

    void registerAxisForY(InputManager& inputManager, const InputSource& source, size_t n)
    {
        if (n < AxisN) axisCallbackEntriesY[n] = inputManager.registerVariableCallback(source, [=](InputSource source, float val)
        {
            valuePos.y = val;
            valueNeg.y = 0;
            if (action) action(valuePos - valueNeg);
        });
    }
};
