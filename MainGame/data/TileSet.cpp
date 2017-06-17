#include "TileSet.hpp"

#include <streamReaders.hpp>

bool readFromStream(std::istream& stream, TileSet& tileSet)
{
    return readFromStream(stream, tileSet.textureName, tileSet.physicalParameters, tileSet.tileModes);
}
