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

#include "LevelData.hpp"

#include <algorithm>

#include "language/LocalizationManager.hpp"
#include "language/LangID.hpp"

bool readRLGrid(sf::InputStream& stream, util::grid<bool>& grid)
{
    size_t width, height;
    if (!readFromStream(stream, varLength(width), varLength(height))) return false;
    
    util::grid<bool> newGrid(width, height);  
    
    auto curIt = newGrid.begin();
    bool curVal = false;
    
    while (curIt != newGrid.end())
    {
        size_t length;
        if (!readFromStream(stream, varLength(length))) return false;
        if (std::distance(curIt, newGrid.end()) < length) return false;
        curIt = std::fill_n(curIt, length, curVal);
        curVal = !curVal;
    }
    
    swap(grid, newGrid);
    return true;
}

bool readBinaryGrid(sf::InputStream& stream, util::grid<bool>& grid)
{
    size_t width, height;
    if (!readFromStream(stream, varLength(width), varLength(height))) return false;
    
    size_t totalSize = (width*height + 7)/8;
    std::unique_ptr<uint8_t[]> vals{new uint8_t[totalSize]};
    if (stream.read((char*)vals.get(), totalSize*sizeof(uint8_t)) != totalSize*sizeof(uint8_t))
        return false;
        
    size_t mask = 1;
    uint8_t* cur = vals.get();
    
    util::grid<bool> newGrid(width, height);
    for (bool& v : newGrid)
    {
        v = (*cur & mask) != 0;
        mask <<= 1;
        if (mask == 256)
        {
            mask = 1;
            cur++;
        }
    }
    
    swap(grid, newGrid);
    return true;
}

bool readFromStream(sf::InputStream& stream, LevelData::MapData& map)
{
    return readFromStream(stream, map.x, map.y) && readRLGrid(stream, map.map);
}

bool readFromStream(sf::InputStream& stream, LevelData& level)
{
    return readFromStream(stream, level.levelNumber, level.startingRoom, level.mapColor,
        level.songResourceName, level.roomResourceNames, level.roomMaps);
}

bool writeRLGrid(OutputStream& stream, const util::grid<bool>& grid)
{
    if (!writeToStream(stream, varLength(grid.width()), varLength(grid.height()))) return false;
    
    bool curVal = false;
    auto curIt = grid.begin();
    while (curIt != grid.end())
    {
        auto newIt = std::find(curIt, grid.end(), !curVal);
        if (!writeToStream(stream, varLength(std::distance(curIt, newIt)))) return false;
        curIt = newIt;
        curVal = !curVal;
    }
    
    return true;
}

bool writeBinaryGrid(OutputStream& stream, const util::grid<bool>& grid)
{
    if (!writeToStream(stream, varLength(grid.width()), varLength(grid.height()))) return false;
    
    size_t totalSize = (grid.width()*grid.height() + 7) / 8;
    std::unique_ptr<uint8_t[]> vals{new uint8_t[totalSize]};
    size_t mask = 1;
    uint8_t* cur = vals.get();

    for (bool v : grid)
    {
        if (mask == 1) *cur = 0;
        if (v) *cur |= mask;
        mask <<= 1;
        if (mask == 256)
        {
            mask = 1;
            cur++;
        }
    }
    
    return stream.write((char*)vals.get(), totalSize*sizeof(uint8_t));
}

bool writeToStream(OutputStream& stream, const LevelData::MapData& map)
{
    return writeToStream(stream, map.x, map.y) && writeRLGrid(stream, map.map);
}

bool writeToStream(OutputStream& stream, const LevelData& level)
{
    return stream.write("LEVEL", 5) && writeToStream(stream, level.levelNumber, level.startingRoom,
        level.mapColor, level.songResourceName, level.roomResourceNames, level.roomMaps);
}

std::string getLevelFileNameForNumber(size_t number)
{
    return (number <= 10 ? "level" + std::to_string(number) : "fboss") + ".map";
}

std::string getLevelNameForNumber(LocalizationManager& lm, size_t number)
{
    if (number <= 10)
        return lm.getFormattedString("level-name", {}, { { "n", number } }, {});
    else return lm.getString("level-name-final-boss");
}
