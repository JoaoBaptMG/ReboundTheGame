#include "InputPlayerController.hpp"

InputPlayerController::InputPlayerController(InputManager &inputManager, const InputSettings &settings)
{
    dashb.registerSource(inputManager, settings.keyboardSettings.dashInput);
    jumpb.registerSource(inputManager, settings.keyboardSettings.jumpInput);
    bombb.registerSource(inputManager, settings.keyboardSettings.bombInput);
    pauseb.registerSource(inputManager, settings.keyboardSettings.pauseInput);
    movementd.registerButtonForX(inputManager, settings.keyboardSettings.moveLeft,  AxisDirection::Negative);
    movementd.registerButtonForX(inputManager, settings.keyboardSettings.moveRight, AxisDirection::Positive);
    movementd.registerButtonForY(inputManager, settings.keyboardSettings.moveUp,    AxisDirection::Negative);
    movementd.registerButtonForY(inputManager, settings.keyboardSettings.moveDown,  AxisDirection::Positive);

    dashb.registerSource(inputManager, settings.joystickSettings.dashInput);
    jumpb.registerSource(inputManager, settings.joystickSettings.jumpInput);
    bombb.registerSource(inputManager, settings.joystickSettings.bombInput);
    pauseb.registerSource(inputManager, settings.joystickSettings.pauseInput);
    movementd.registerAxisForX(inputManager, settings.joystickSettings.movementAxisX);
    movementd.registerAxisForY(inputManager, settings.joystickSettings.movementAxisY);
}

void InputPlayerController::update()
{
    dashb.update();
    jumpb.update();
    bombb.update();
    pauseb.update();
}
