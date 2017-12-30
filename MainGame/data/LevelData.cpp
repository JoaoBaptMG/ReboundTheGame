#include "LevelData.hpp"

#include <algorithm>

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
    return readFromStream(stream, level.levelNumber, level.startingRoom, level.songResourceName,
        level.roomResourceNames, level.roomMaps);
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
        level.songResourceName, level.roomResourceNames, level.roomMaps);
}
