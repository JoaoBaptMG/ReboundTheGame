#include "InputPlayerController.hpp"

InputPlayerController::InputPlayerController(InputManager &inputManager, const InputSettings &settings)
{
    dashb.registerSource(inputManager, settings.keyboardSettings.dashInput, 0);
    jumpb.registerSource(inputManager, settings.keyboardSettings.jumpInput, 0);
    bombb.registerSource(inputManager, settings.keyboardSettings.bombInput, 0);
    pauseb.registerSource(inputManager, settings.keyboardSettings.pauseInput, 0);
    movementd.registerButtonForX(inputManager, settings.keyboardSettings.moveLeft,  AxisDirection::Negative, 0);
    movementd.registerButtonForX(inputManager, settings.keyboardSettings.moveRight, AxisDirection::Positive, 0);
    movementd.registerButtonForY(inputManager, settings.keyboardSettings.moveUp,    AxisDirection::Negative, 0);
    movementd.registerButtonForY(inputManager, settings.keyboardSettings.moveDown,  AxisDirection::Positive, 0);

    dashb.registerSource(inputManager, settings.joystickSettings.dashInput, 1);
    jumpb.registerSource(inputManager, settings.joystickSettings.jumpInput, 1);
    bombb.registerSource(inputManager, settings.joystickSettings.bombInput, 1);
    pauseb.registerSource(inputManager, settings.joystickSettings.pauseInput, 1);
    movementd.registerAxisForX(inputManager, settings.joystickSettings.movementAxisX, 0);
    movementd.registerAxisForY(inputManager, settings.joystickSettings.movementAxisY, 0);
}

void InputPlayerController::update()
{
    dashb.update();
    jumpb.update();
    bombb.update();
    pauseb.update();
}
