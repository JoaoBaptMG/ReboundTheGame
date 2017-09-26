#include "RoomData.hpp"

#include <streamReaders.hpp>
#include "objects/GameObject.hpp"

bool readFromStream(sf::InputStream& stream, GameObjectDescriptor& descriptor)
{
    if (!readFromStream(stream, descriptor.klass)) return false;
    descriptor.parameters = readParametersFromStream(stream, descriptor.klass);
    return !descriptor.parameters.empty();
}

bool readFromStream(sf::InputStream &stream, RoomData& room)
{
    return readFromStream(stream, room.tilesetName, room.mainLayer, room.gameObjectDescriptors, room.warps);
}
