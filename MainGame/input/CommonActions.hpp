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

template <size_t N>
class AxisAction final : public AxisActionBase
{
    InputManager::CallbackEntry callbackEntries[3*N];

public:
    AxisAction() {}
    ~AxisAction() {}

    void registerAxis(InputManager& inputManager, const InputSource& source, size_t n)
    {
        if (n < N) callbackEntries[3*n] = inputManager.registerVariableCallback(source, [=](InputSource source, float val)
            {
                valuePos = val;
                valueNeg = 0;
                if (action) action(valuePos - valueNeg);
            });
    }

    void registerButton(InputManager& inputManager, const InputSource& source, AxisDirection dir, size_t n)
    {
        if (n < N) callbackEntries[3*n + (dir == AxisDirection::Positive ? 2 : 1)] =
                       inputManager.registerVariableCallback(source, [=](InputSource source, float val)
                       {
                           if (dir == AxisDirection::Positive) valuePos = val; else valueNeg = val;
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

template <size_t N>
class DualAxisAction final : public DualAxisActionBase
{
    InputManager::CallbackEntry callbackEntries[6*N];

public:
    void registerAxisForX(InputManager& inputManager, const InputSource& source, size_t n)
    {
        if (n < N) callbackEntries[6*n] = inputManager.registerVariableCallback(source, [=](InputSource source, float val)
            {
                valuePos.x = val;
                valueNeg.x = 0;
                if (action) action(valuePos - valueNeg);
            });
    }

    void registerAxisForY(InputManager& inputManager, const InputSource& source, size_t n)
    {
        if (n < N) callbackEntries[6*n+3] = inputManager.registerVariableCallback(source, [=](InputSource source, float val)
            {
                valuePos.y = val;
                valueNeg.y = 0;
                if (action) action(valuePos - valueNeg);
            });
    }

    void registerButtonForX(InputManager& inputManager, const InputSource& source, AxisDirection dir, size_t n)
    {
        if (n < N) callbackEntries[6*n + (dir == AxisDirection::Positive ? 2 : 1)] =
                       inputManager.registerVariableCallback(source, [=](InputSource source, float val)
                       {
                           if (dir == AxisDirection::Positive) valuePos.x = val; else valueNeg.x = val;
                           if (action) action(valuePos - valueNeg);
                       });
    }

    void registerButtonForY(InputManager& inputManager, const InputSource& source, AxisDirection dir, size_t n)
    {
        if (n < N) callbackEntries[6*n + (dir == AxisDirection::Negative ? 5 : 4)] =
                       inputManager.registerVariableCallback(source, [=](InputSource source, float val)
                       {
                           if (dir == AxisDirection::Positive) valuePos.y = val; else valueNeg.y = val;
                           if (action) action(valuePos - valueNeg);
                       });
    }
};