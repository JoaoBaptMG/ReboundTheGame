#include "PlayerController.hpp"

PlayerController::PlayerController(InputManager &inputManager)
{
    leftEntry = inputManager.registerCallback(Actions::Left,
        [this](ActionState state) { isLeft = state == ActionState::Down; });
    rightEntry = inputManager.registerCallback(Actions::Right,
        [this](ActionState state) { isRight = state == ActionState::Down; });
    upEntry = inputManager.registerCallback(Actions::Up,
        [this](ActionState state) { isUp = state == ActionState::Down; });
    downEntry = inputManager.registerCallback(Actions::Down,
        [this](ActionState state) { isDown = state == ActionState::Down; });
    jumpEntry = inputManager.registerCallback(Actions::Jump,
        [this](ActionState state) { curJump = state == ActionState::Down; });
    dashEntry = inputManager.registerCallback(Actions::Dash,
        [this](ActionState state) { curDash = state == ActionState::Down; });
    bombEntry = inputManager.registerCallback(Actions::Bomb,
        [this](ActionState state) { curBomb = state == ActionState::Down; });
}

void PlayerController::update()
{
    moveVector.x = isRight ? 1 : isLeft ? -1 : 0;
    moveVector.y = isDown ? 1 : isUp ? -1 : 0;

    lastJump2 = lastJump1;
    lastJump1 = curJump;

    lastDash2 = lastDash1;
    lastDash1 = curDash;

    lastBomb2 = lastBomb1;
    lastBomb1 = curBomb;
}
