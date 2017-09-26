#include "LevelData.hpp"

bool readFromStream(sf::InputStream& stream, LevelData& level)
{
    return readFromStream(stream, level.levelNumber, level.startingRoom,
                                  level.songResourceName, level.roomResourceNames);
}

