#include "RoomData.hpp"
#include <cstring>
#include <cstdint>
#include "../utility/streamCommons.hpp"

bool readFromStream(sf::InputStream& stream, GameObjectDescriptor& descriptor)
{
    using namespace util;

    uint32_t size;
    if (!readFromStream(stream, descriptor.klass, size)) return false;
    descriptor.parameters = Memory(size);
    return stream.read(descriptor.parameters.get_ptr(), size) == size;
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
