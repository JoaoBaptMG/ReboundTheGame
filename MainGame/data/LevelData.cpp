#include "LevelData.hpp"

bool readFromStream(sf::InputStream& stream, LevelData& level)
{
    using namespace util;

    return readFromStream(stream, level.levelNumber, level.startingRoom, level.levelName,
                                  level.songResourceName, level.roomResourceNames);
}

bool LevelData::loadFromStream(sf::InputStream& stream)
{
    return util::checkMagic(stream, "LEVEL") && readFromStream(stream, *this);
}
