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

#include "Settings.hpp"

bool readFromStream(InputStream &stream, InputSettings& settings)
{
    return readFromStream(stream, settings.keyboardSettings.dashInput,
                                  settings.keyboardSettings.jumpInput,
                                  settings.keyboardSettings.bombInput,
                                  settings.keyboardSettings.switchScreenLeft,
                                  settings.keyboardSettings.switchScreenRight,
                                  settings.keyboardSettings.pauseInput,
                                  settings.keyboardSettings.okInput,
                                  settings.keyboardSettings.cancelInput,
                                  settings.keyboardSettings.moveUp,
                                  settings.keyboardSettings.moveDown,
                                  settings.keyboardSettings.moveLeft,
                                  settings.keyboardSettings.moveRight,

                                  settings.joystickSettings.dashInput,
                                  settings.joystickSettings.jumpInput,
                                  settings.joystickSettings.bombInput,
                                  settings.joystickSettings.switchScreenLeft,
                                  settings.joystickSettings.switchScreenRight,
                                  settings.joystickSettings.pauseInput,
                                  settings.joystickSettings.okInput,
                                  settings.joystickSettings.cancelInput,
                                  settings.joystickSettings.movementAxisX,
                                  settings.joystickSettings.movementAxisY,
                                  settings.joystickSettings.movementAxisXAlt,
                                  settings.joystickSettings.movementAxisYAlt);
}

bool writeToStream(OutputStream& stream, const InputSettings& settings)
{
    return writeToStream(stream, settings.keyboardSettings.dashInput,
                                 settings.keyboardSettings.jumpInput,
                                 settings.keyboardSettings.bombInput,
                                 settings.keyboardSettings.switchScreenLeft,
                                 settings.keyboardSettings.switchScreenRight,
                                 settings.keyboardSettings.pauseInput,
                                 settings.keyboardSettings.okInput,
                                 settings.keyboardSettings.cancelInput,
                                 settings.keyboardSettings.moveUp,
                                 settings.keyboardSettings.moveDown,
                                 settings.keyboardSettings.moveLeft,
                                 settings.keyboardSettings.moveRight,

                                 settings.joystickSettings.dashInput,
                                 settings.joystickSettings.jumpInput,
                                 settings.joystickSettings.bombInput,
                                 settings.joystickSettings.switchScreenLeft,
                                 settings.joystickSettings.switchScreenRight,
                                 settings.joystickSettings.pauseInput,
                                 settings.joystickSettings.okInput,
                                 settings.joystickSettings.cancelInput,
                                 settings.joystickSettings.movementAxisX,
                                 settings.joystickSettings.movementAxisY,
                                 settings.joystickSettings.movementAxisXAlt,
                                 settings.joystickSettings.movementAxisYAlt);
}

bool readFromStream(InputStream &stream, VideoSettings& settings)
{
    return readFromStream(stream, settings.fullscreen, settings.vsyncEnabled);
}

bool writeToStream(OutputStream& stream, const VideoSettings& settings)
{
    return writeToStream(stream, settings.fullscreen, settings.vsyncEnabled);
}

bool readFromStream(InputStream &stream, AudioSettings& settings)
{
    return readFromStream(stream, settings.musicVolume, settings.soundVolume);
}

bool writeToStream(OutputStream& stream, const AudioSettings& settings)
{
    return writeToStream(stream, settings.musicVolume, settings.soundVolume);
}

bool readFromStream(InputStream &stream, KeyPair& keyPair)
{
    return readFromStream(stream, keyPair.name, keyPair.key);
}

bool writeToStream(OutputStream& stream, const KeyPair& keyPair)
{
    return writeToStream(stream, keyPair.name, keyPair.key);
}
