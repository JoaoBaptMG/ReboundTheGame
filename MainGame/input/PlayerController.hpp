#pragma once

#include "InputManager.hpp"
#include "../utility/non_copyable_movable.hpp"
#include <SFML/System.hpp>

class PlayerController final : util::non_copyable
{
    InputManager::CallbackEntry leftEntry, rightEntry, upEntry, downEntry, jumpEntry;

    bool curJump, isLeft, isRight, isUp, isDown;
    bool lastJump1, lastJump2;
    sf::Vector2f moveVector;

public:
    enum Actions { Left, Right, Up, Down, Jump };

    explicit PlayerController(InputManager &manager);

    void update();

    inline bool isJumpPressed() const { return active && lastJump1 && !lastJump2; }
    inline bool isJumpReleased() const { return active && !lastJump1 && lastJump2; }
    
    inline sf::Vector2f getMovementVector() const { return moveVector; }

    bool active;
};

