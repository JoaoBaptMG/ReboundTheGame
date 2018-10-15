//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//


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
    movementd.registerAxisForX(inputManager, settings.joystickSettings.movementAxisXAlt, 1);
    movementd.registerAxisForY(inputManager, settings.joystickSettings.movementAxisYAlt, 1);
}

void InputPlayerController::update()
{
    dashb.update();
    jumpb.update();
    bombb.update();
    pauseb.update();
}
