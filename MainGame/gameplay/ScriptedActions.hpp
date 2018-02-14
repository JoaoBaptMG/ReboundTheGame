#pragma once

#include "VirtualActions.hpp"

class ScriptedButtonAction : public VirtualButtonAction
{
public:
    virtual bool isTriggered() const override;
    virtual bool isPressed() const override;
    virtual bool isReleased() const override;
};

class ScriptedDualAxisAction : public VirtualDualAxisAction
{
public:
    virtual sf::Vector2f getValue() const override;
};
