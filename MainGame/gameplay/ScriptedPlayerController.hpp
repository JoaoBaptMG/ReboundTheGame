#pragma once

#include "PlayerController.hpp"
#include "ScriptedActions.hpp"

class ScriptedPlayerController : public PlayerController
{
    ScriptedButtonAction dummy;
    ScriptedDualAxisAction dummyMovement;
    
public:
    virtual const VirtualButtonAction& jump() const { return dummy; };
    virtual const VirtualButtonAction& dash() const { return dummy; };
    virtual const VirtualButtonAction& bomb() const { return dummy; };
    virtual const VirtualButtonAction& pause() const { return dummy; };
    virtual const VirtualDualAxisAction& movement() const { return dummyMovement; };
};
