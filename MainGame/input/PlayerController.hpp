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

    friend void swap(PlayerController &pc1, PlayerController &pc2) noexcept
    {
        using std::swap;
        swap(pc1.leftEntry, pc2.leftEntry);
        swap(pc1.rightEntry, pc2.rightEntry);
        swap(pc1.upEntry, pc2.upEntry);
        swap(pc1.downEntry, pc2.downEntry);
        swap(pc1.jumpEntry, pc2.jumpEntry);

        swap(pc1.lastJump1, pc2.lastJump2);
        swap(pc1.lastJump1, pc2.lastJump2);
        swap(pc1.curJump, pc2.curJump);
        swap(pc1.isLeft, pc2.isLeft);
        swap(pc1.isRight, pc2.isRight);
        swap(pc1.isUp, pc2.isUp);
        swap(pc1.isDown, pc2.isDown);

        swap(pc1.active, pc2.active);
    }

    inline bool hasJumpAction() const { return active && lastJump1 && !lastJump2; }
    inline sf::Vector2f getMovementVector() const { return moveVector; }

    bool active;
};

