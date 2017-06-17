#pragma once

#include "InputManager.hpp"
#include "CommonActions.hpp"
#include <non_copyable_movable.hpp>
#include <SFML/System.hpp>

class PlayerController final : util::non_copyable
{
public:
    explicit PlayerController(InputManager &manager);

    ButtonAction jump, dash, bomb;
    DualAxisAction movement;

    void update();

    bool active = true;
};

