#pragma once

#include "VirtualActions.hpp"

class PlayerController
{
public:
    virtual const VirtualButtonAction& jump() const = 0;
    virtual const VirtualButtonAction& dash() const = 0;
    virtual const VirtualButtonAction& bomb() const = 0;
    virtual const VirtualButtonAction& pause() const = 0;
    virtual const VirtualDualAxisAction& movement() const = 0;
};
