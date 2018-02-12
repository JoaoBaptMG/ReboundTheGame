#pragma once

#include "InputManager.hpp"
#include "CommonActions.hpp"
#include "settings/InputSettings.hpp"
#include <non_copyable_movable.hpp>
#include <SFML/System.hpp>

class PlayerController final : util::non_copyable
{
public:
    explicit PlayerController(InputManager &manager, const InputSettings &settings);

    ButtonAction jump, dash, bomb, pause;
    DualAxisAction movement;

    void update();

    bool active = true;
};

