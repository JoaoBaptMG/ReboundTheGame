#pragma once

#include "utility/non_copyable_movable.hpp"
#include "utility/streamCommons.hpp"

#include <vector>
#include <SFML/System.hpp>

class LevelData final
{
public:
    uint16_t levelNumber;
    uint16_t startingRoom;
    std::string songResourceName;
    std::vector<std::string> roomResourceNames;

    bool loadFromStream(sf::InputStream& stream);
};

bool readFromStream(sf::InputStream& stream, LevelData& level);
