#include "Settings.hpp"
#include "execDir.hpp"
#include "FileOutputStream.hpp"

constexpr auto SettingsFileName = "settings.sav";

static Settings defaultSettings()
{
    static Settings defaults
    {
        {
            {
                InputSource::keyboardKey((sf::Keyboard::Key)79),
                InputSource::keyboardKey((sf::Keyboard::Key)80),
                InputSource::keyboardKey((sf::Keyboard::Key)81),
                InputSource::keyboardKey(sf::Keyboard::Key::W),
                InputSource::keyboardKey(sf::Keyboard::Key::S),
                InputSource::keyboardKey(sf::Keyboard::Key::A),
                InputSource::keyboardKey(sf::Keyboard::Key::D)
            },

            {
                InputSource::joystickButton(0, 0),
                InputSource::joystickButton(0, 1),
                InputSource::joystickButton(0, 2),
                InputSource::joystickAxis(0, sf::Joystick::Axis::X),
                InputSource::joystickAxis(0, sf::Joystick::Axis::Y),
            }
        },

        { (uint8_t)false, (uint8_t)true },
        { 100, 100 },
    };
    return defaults;
}

Settings loadSettingsFile(bool *success)
{
    if (success) *success = false;
    
    auto fullname = getExecutableDirectory() + '/' + SettingsFileName;
    sf::FileInputStream file;

    if (!file.open(fullname)) return defaultSettings();
    if (!checkMagic(file, "SETTINGS")) return defaultSettings();

    size_t fileVersion;
    if (!readFromStream(file, VarLength(fileVersion)))
        return defaultSettings();
    if (fileVersion != SettingsVersion)
        return defaultSettings();

    Settings settings = defaultSettings();
    if (!readFromStream(file, settings.inputSettings, settings.videoSettings, settings.audioSettings))
        return defaultSettings();

    if (success) *success = true;
    return settings;
}

bool storeSettingsFile(const Settings& settings)
{
    auto fullname = getExecutableDirectory() + '/' + SettingsFileName;
    FileOutputStream file;

    size_t fileVersion = SettingsVersion;
    if (!file.open(fullname)) return false;
    return writeMagic(file, "SETTINGS") && writeToStream(file, VarLength(fileVersion),
        settings.inputSettings, settings.videoSettings, settings.audioSettings);
}
