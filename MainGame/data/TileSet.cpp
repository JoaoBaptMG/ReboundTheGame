#include "TileSet.hpp"

#include "utility/streamCommons.hpp"

bool readFromStream(sf::InputStream& stream, TileSet& tileSet)
{
    using namespace util;

    return readFromStream(stream, tileSet.textureName, tileSet.physicalParameters, tileSet.tileModes);
}

bool TileSet::loadFromStream(sf::InputStream& stream)
{
    return util::checkMagic(stream, "TSET") && readFromStream(stream, *this);
}
