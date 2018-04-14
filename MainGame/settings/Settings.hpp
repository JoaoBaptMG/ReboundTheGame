#pragma once

#include <string>
#include <vector>
#include <utility>
#include <SFML/System.hpp>
#include <OutputStream.hpp>
#include "InputSettings.hpp"
#include "VideoSettings.hpp"
#include "AudioSettings.hpp"
#include "gameplay/SavedGame.hpp"

constexpr size_t SettingsVersion = 0;

struct KeyPair
{
    std::string name;
    SavedGame::Key key;
    
    KeyPair(std::string name, SavedGame::Key key) : name(name), key(key) {}
    KeyPair() {}
};

struct Settings
{
    InputSettings inputSettings;
    VideoSettings videoSettings;
    AudioSettings audioSettings;
    std::string languageFile;
    std::vector<KeyPair> savedKeys;
};

bool readFromStream(sf::InputStream &stream, KeyPair& keyPair);
bool writeToStream(OutputStream& stream, const KeyPair& keyPair);

bool readFromStream(sf::InputStream &stream, Settings& settings);
bool writeToStream(OutputStream& stream, const Settings& settings);

Settings loadSettingsFile(bool *success = nullptr);
bool storeSettingsFile(const Settings& settings);
