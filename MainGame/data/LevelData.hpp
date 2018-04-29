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

#include <non_copyable_movable.hpp>
#include <streamReaders.hpp>
#include <streamWriters.hpp>
#include <grid.hpp>

#include <vector>
#include <string>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

class LocalizationManager;

class LevelData final
{
public:
    uint16_t levelNumber;
    uint16_t startingRoom;
    sf::Color mapColor;
    std::string songResourceName;
    std::vector<std::string> roomResourceNames;
    
    struct MapData
    {
        int16_t x = 0, y = 0;
        util::grid<bool> map;
    };
    
    std::vector<MapData> roomMaps;

    static constexpr auto ReadMagic = "LEVEL";
};

bool readRLGrid(sf::InputStream& stream, util::grid<bool>& grid);
bool readBinaryGrid(sf::InputStream& stream, util::grid<bool>& grid);
bool readFromStream(sf::InputStream& stream, LevelData::MapData& map);
bool readFromStream(sf::InputStream& stream, LevelData& level);

bool writeRLGrid(OutputStream& stream, const util::grid<bool>& grid);
bool writeBinaryGrid(OutputStream& stream, const util::grid<bool>& grid);
bool writeToStream(OutputStream& stream, const LevelData::MapData& map);
bool writeToStream(OutputStream& stream, const LevelData& level);

std::string getLevelFileNameForNumber(size_t number);
std::string getLevelNameForNumber(LocalizationManager& lm, size_t number);
