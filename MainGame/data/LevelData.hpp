#pragma once

#include <non_copyable_movable.hpp>
#include <streamReaders.hpp>
#include <streamWriters.hpp>
#include <grid.hpp>

#include <vector>
#include <SFML/System.hpp>

class LevelData final
{
public:
    uint16_t levelNumber;
    uint16_t startingRoom;
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
