#pragma once

#include "utility/non_copyable_movable.hpp"
#include "utility/streamCommons.hpp"

#include <vector>
#include <SFML/System.hpp>

class LevelData final
{
public:
    uint32_t levelNumber;
    uint32_t startingRoom;
    std::string levelName;
    std::string songResourceName;
    std::vector<std::string> roomResourceNames;

    LevelData() : mainLayer(), textureName() {}
    LevelData(LevelData&& other) noexcept : LevelData() { swap(*this, other); }
    ~LevelData() {}

    LevelData& operator=(LevelData&& other) noexcept { swap(*this, other); return *this; }

    friend void swap(LevelData& r1, LevelData &r2)
    {
        using std::swap;
        swap(r1.levelNumver, r2.levelNumber);
        swap(r1.levelName, r2.levelName);
        swap(r1.songName, r2.songName);
        swap(r1.rooms, r2.rooms);
    }

    bool loadFromStream(sf::InputStream& stream);
}

bool readFromStream(sf::InputStream& stream, LevelData& level);
