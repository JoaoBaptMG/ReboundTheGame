#pragma once

#include "input/InputSource.hpp"

struct KeyboardSettings
{
    InputSource dashInput;
    InputSource jumpInput;
    InputSource bombInput;
    InputSource switchScreenLeft;
    InputSource switchScreenRight;
    InputSource pauseInput;

    InputSource moveUp;
    InputSource moveDown;
    InputSource moveLeft;
    InputSource moveRight;
};

struct JoystickSettings
{
    InputSource dashInput;
    InputSource jumpInput;
    InputSource bombInput;
    InputSource switchScreenLeft;
    InputSource switchScreenRight;
    InputSource pauseInput;

    InputSource movementAxisX;
    InputSource movementAxisY;
};

struct InputSettings
{
    KeyboardSettings keyboardSettings;
    JoystickSettings joystickSettings;
};

bool readFromStream(sf::InputStream &stream, InputSettings& settings);
bool writeToStream(OutputStream& stream, const InputSettings& settings);
