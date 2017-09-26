#include "TileSet.hpp"

#include <streamReaders.hpp>

bool readFromStream(sf::InputStream& stream, TileSet& tileSet)
{
    return readFromStream(stream, tileSet.textureName, tileSet.physicalParameters, tileSet.tileModes);
}
