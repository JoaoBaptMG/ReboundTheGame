#pragma once

#include <string>
#include <vector>
#include <SFML/System.hpp>
#include <OutputStream.hpp>
#include "InputSettings.hpp"
#include "VideoSettings.hpp"
#include "AudioSettings.hpp"
#include "gameplay/SavedGame.hpp"

constexpr size_t SettingsVersion = 3;

struct Settings
{
    InputSettings inputSettings;
    VideoSettings videoSettings;
    AudioSettings audioSettings;
    std::string languageFile;
    std::vector<SavedGame::Key> savedKeys;
};

bool readFromStream(sf::InputStream &stream, Settings& settings);
bool writeToStream(OutputStream& stream, const Settings& settings);

Settings loadSettingsFile(bool *success = nullptr);
bool storeSettingsFile(const Settings& settings);
