#include "RoomData.hpp"
#include <cstring>
#include <cstdint>
#include "../utility/streamCommons.hpp"

bool RoomData::loadFromStream(sf::InputStream &stream)
{
    using namespace util;

    return checkMagic(stream, "ROOM")
        && readFromStream(stream, textureName, mainLayer);
}
