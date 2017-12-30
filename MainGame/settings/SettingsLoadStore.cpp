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
#if _WIN32
#elif __APPLE__
#elif __linux__
                InputSource::keyboardKey(83),
                InputSource::keyboardKey(84),
                InputSource::keyboardKey(85),
                InputSource::keyboardKey(25),
                InputSource::keyboardKey(39),
                InputSource::keyboardKey(38),
                InputSource::keyboardKey(40)
#endif
            },

            {
                InputSource::joystickButton(0),
                InputSource::joystickButton(1),
                InputSource::joystickButton(2),
                InputSource::joystickAxis(sf::Joystick::Axis::X),
                InputSource::joystickAxis(sf::Joystick::Axis::Y),
            }
        },

        { (uint8_t)false, (uint8_t)true },
        { 100, 100 },
        "",
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
    if (!readFromStream(file, varLength(fileVersion)))
        return defaultSettings();
    if (fileVersion != SettingsVersion)
        return defaultSettings();

    Settings settings = defaultSettings();
    if (!readFromStream(file, settings.inputSettings, settings.videoSettings, settings.audioSettings,
        settings.languageFile))
        return defaultSettings();

    if (success) *success = true;
    return settings;
}

bool storeSettingsFile(const Settings& settings)
{
    auto fullname = getExecutableDirectory() + '/' + SettingsFileName;
    FileOutputStream file;

    if (!file.open(fullname)) return false;
    return writeMagic(file, "SETTINGS") && writeToStream(file, varLength(SettingsVersion),
        settings.inputSettings, settings.videoSettings, settings.audioSettings, settings.languageFile);
}
