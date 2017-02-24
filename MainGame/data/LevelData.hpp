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

    LevelData() {}
    LevelData(LevelData&& other) noexcept : LevelData() { swap(*this, other); }
    ~LevelData() {}

    LevelData& operator=(LevelData&& other) noexcept { swap(*this, other); return *this; }

    friend void swap(LevelData& r1, LevelData &r2)
    {
        using std::swap;
        swap(r1.levelNumber, r2.levelNumber);
        swap(r1.songResourceName, r2.songResourceName);
        swap(r1.roomResourceNames, r2.roomResourceNames);
    }

    bool loadFromStream(sf::InputStream& stream);
};

bool readFromStream(sf::InputStream& stream, LevelData& level);
