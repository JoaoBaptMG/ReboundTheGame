#pragma once

#include <non_copyable_movable.hpp>
#include <streamReaders.hpp>

#include <vector>
#include <SFML/System.hpp>

class LevelData final
{
public:
    uint16_t levelNumber;
    uint16_t startingRoom;
    std::string songResourceName;
    std::vector<std::string> roomResourceNames;

    static constexpr auto ReadMagic = "LEVEL";
};

bool readFromStream(sf::InputStream& stream, LevelData& level);
