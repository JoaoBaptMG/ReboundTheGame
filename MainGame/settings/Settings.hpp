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
