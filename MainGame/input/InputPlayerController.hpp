#pragma once

#include "gameplay/PlayerController.hpp"
#include "InputManager.hpp"
#include "CommonActions.hpp"
#include "settings/InputSettings.hpp"
#include <non_copyable_movable.hpp>
#include <SFML/System.hpp>

class InputPlayerController final : public PlayerController, util::non_copyable
{
public:
    InputPlayerController(InputManager &manager, const InputSettings &settings);

    ButtonAction<2> jumpb, dashb, bombb, pauseb;
    DualAxisAction<1> movementd;

    void update();
    
    virtual const VirtualButtonAction& jump() const override { return jumpb; }
    virtual const VirtualButtonAction& dash() const override { return dashb; }
    virtual const VirtualButtonAction& bomb() const override { return bombb; }
    virtual const VirtualButtonAction& pause() const override { return pauseb; }
    virtual const VirtualDualAxisAction& movement() const override { return movementd; }

    bool active = true;
};

