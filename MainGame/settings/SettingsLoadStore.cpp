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
#include "execDir.hpp"
#include <streams/FileInputStream.hpp>
#include <streams/FileOutputStream.hpp>

constexpr auto SettingsFileName = "settings.sav";

static Settings defaultSettings()
{
    static Settings defaults
    {
        {
            {
#if _WIN32
				InputSource::keyboardKey(75),
				InputSource::keyboardKey(76),
				InputSource::keyboardKey(77),

				InputSource::keyboardKey(71),
				InputSource::keyboardKey(73),
				InputSource::keyboardKey(1),
				InputSource::keyboardKey(76),
				InputSource::keyboardKey(75),

				InputSource::keyboardKey(17),
				InputSource::keyboardKey(31),
				InputSource::keyboardKey(30),
				InputSource::keyboardKey(32),
#elif __APPLE__
                InputSource::keyboardKey(0x56),
                InputSource::keyboardKey(0x57),
                InputSource::keyboardKey(0x58),
                InputSource::keyboardKey(0x59),

                InputSource::keyboardKey(0x56),
                InputSource::keyboardKey(0x35),
                InputSource::keyboardKey(0x0d),
                InputSource::keyboardKey(0x56),
                InputSource::keyboardKey(0x57),

                InputSource::keyboardKey(0x01),
                InputSource::keyboardKey(0x00),
                InputSource::keyboardKey(0x02),
#elif __linux__
                InputSource::keyboardKey(83),
                InputSource::keyboardKey(84),
                InputSource::keyboardKey(85),
                
                InputSource::keyboardKey(79),
                InputSource::keyboardKey(81),
                InputSource::keyboardKey(9),
                InputSource::keyboardKey(83),
                InputSource::keyboardKey(84),

                InputSource::keyboardKey(25),
                InputSource::keyboardKey(39),
                InputSource::keyboardKey(38),
                InputSource::keyboardKey(40),
#endif
            },

            {
                InputSource::joystickButton(0),
                InputSource::joystickButton(1),
                InputSource::joystickButton(2),

                InputSource::joystickButton(3),
                InputSource::joystickButton(4),
                InputSource::joystickButton(5),
                InputSource::joystickButton(0),
                InputSource::joystickButton(1),

                InputSource::joystickAxis(sf::Joystick::Axis::X),
                InputSource::joystickAxis(sf::Joystick::Axis::Y),
                InputSource::none,
                InputSource::none
            }
        },

        { (uint8_t)false, (uint8_t)true },
        { 100, 100 },
        "",
        {},
    };
    return defaults;
}

Settings loadSettingsFile(bool *success)
{
    if (success) *success = false;
    
    auto fullname = getExecutableDirectory() + '/' + SettingsFileName;
    FileInputStream file;

    if (!file.open(fullname)) return defaultSettings();
    if (!checkMagic(file, "SETTINGS")) return defaultSettings();

    size_t fileVersion;
    if (!readFromStream(file, varLength(fileVersion)))
        return defaultSettings();
    if (fileVersion != SettingsVersion)
        return defaultSettings();

    Settings settings = defaultSettings();
    if (!readFromStream(file, settings.inputSettings, settings.videoSettings, settings.audioSettings,
        settings.languageFile, settings.savedKeys))
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
        settings.inputSettings, settings.videoSettings, settings.audioSettings, settings.languageFile,
        settings.savedKeys);
}
