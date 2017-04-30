#pragma once

#include "InputManager.hpp"
#include "../utility/non_copyable_movable.hpp"
#include <SFML/System.hpp>

class PlayerController final : util::non_copyable
{
    InputManager::CallbackEntry leftEntry, rightEntry, upEntry, downEntry, jumpEntry, dashEntry, bombEntry;

    bool isLeft = false, isRight = false, isUp = false, isDown = false;
    bool curJump = false, lastJump1 = false, lastJump2 = false;
    bool curDash = false, lastDash1 = false, lastDash2 = false;
    bool curBomb = false, lastBomb1 = false, lastBomb2 = false;
    sf::Vector2f moveVector{};

public:
    enum Actions { Left, Right, Up, Down, Jump, Dash, Bomb };

    explicit PlayerController(InputManager &manager);

    void update();

    inline bool isJumpPressed() const { return active && lastJump1 && !lastJump2; }
    inline bool isJumpReleased() const { return active && !lastJump1 && lastJump2; }

    inline bool isDashPressed() const { return active && lastDash1 && !lastDash2; }
    inline bool isDashReleased() const { return active && !lastDash1 && lastDash2; }

    inline bool isBombPressed() const { return active && lastBomb1 && !lastBomb2; }
    inline bool isBombReleased() const { return active && !lastBomb1 && lastBomb2; }
    
    inline sf::Vector2f getMovementVector() const { return moveVector; }

    bool active = true;
};

