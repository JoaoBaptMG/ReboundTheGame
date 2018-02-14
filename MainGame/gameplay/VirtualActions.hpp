#pragma once

#include <SFML/System.hpp>

class VirtualButtonAction
{
public:
    virtual bool isTriggered() const = 0;
    virtual bool isPressed() const = 0;
    virtual bool isReleased() const = 0;
};

class VirtualAxisAction
{
public:
    virtual float getValue() const = 0;
};

class VirtualDualAxisAction
{
public:
    virtual sf::Vector2f getValue() const = 0;
};
