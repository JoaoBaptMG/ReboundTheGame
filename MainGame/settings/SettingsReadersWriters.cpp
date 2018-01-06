#include "Settings.hpp"

bool readFromStream(sf::InputStream &stream, InputSettings& settings)
{
    return readFromStream(stream, settings.keyboardSettings.dashInput,
                                  settings.keyboardSettings.jumpInput,
                                  settings.keyboardSettings.bombInput,
                                  settings.keyboardSettings.switchScreenLeft,
                                  settings.keyboardSettings.switchScreenRight,
                                  settings.keyboardSettings.pauseInput,
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
                                  settings.joystickSettings.movementAxisX,
                                  settings.joystickSettings.movementAxisY);
}

bool writeToStream(OutputStream& stream, const InputSettings& settings)
{
    return writeToStream(stream, settings.keyboardSettings.dashInput,
                                 settings.keyboardSettings.jumpInput,
                                 settings.keyboardSettings.bombInput,
                                 settings.keyboardSettings.switchScreenLeft,
                                 settings.keyboardSettings.switchScreenRight,
                                 settings.keyboardSettings.pauseInput,
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
                                 settings.joystickSettings.movementAxisX,
                                 settings.joystickSettings.movementAxisY);
}

bool readFromStream(sf::InputStream &stream, VideoSettings& settings)
{
    return readFromStream(stream, settings.fullscreen, settings.vsyncEnabled);
}

bool writeToStream(OutputStream& stream, const VideoSettings& settings)
{
    return writeToStream(stream, settings.fullscreen, settings.vsyncEnabled);
}

bool readFromStream(sf::InputStream &stream, AudioSettings& settings)
{
    return readFromStream(stream, settings.musicVolume, settings.soundVolume);
}

bool writeToStream(OutputStream& stream, const AudioSettings& settings)
{
    return writeToStream(stream, settings.musicVolume, settings.soundVolume);
}
