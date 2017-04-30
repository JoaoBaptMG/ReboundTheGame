#include "RoomData.hpp"
#include <cstring>
#include <cstdint>
#include "utility/streamCommons.hpp"
#include "objects/GameObject.hpp"

bool readFromStream(sf::InputStream& stream, GameObjectDescriptor& descriptor)
{
    using namespace util;

    if (!readFromStream(stream, descriptor.klass)) return false;
    descriptor.parameters = readParametersFromStream(stream, descriptor.klass);
    return !descriptor.parameters.empty();
}

bool readFromStream(sf::InputStream &stream, RoomData& room)
{
    using namespace util;

    return readFromStream(stream, room.textureName, room.mainLayer, room.gameObjectDescriptors, room.warps);
}

bool RoomData::loadFromStream(sf::InputStream& stream)
{
    return util::checkMagic(stream, "ROOM") && readFromStream(stream, *this);
}
