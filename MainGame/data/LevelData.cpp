#include "LevelData.hpp"

bool readFromStream(std::istream& stream, LevelData& level)
{
    return readFromStream(stream, level.levelNumber, level.startingRoom,
                                  level.songResourceName, level.roomResourceNames);
}

