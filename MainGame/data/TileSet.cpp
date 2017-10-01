#include "TileSet.hpp"

#include <streamReaders.hpp>
#include <algorithm>

template <typename T>
bool readFromStream(sf::InputStream& stream, sf::Vector2<T> &vec)
{
    return readFromStream(stream, vec.x, vec.y);
}

bool readFromStream(sf::InputStream& stream, TileSet::Terrain& terrain)
{
    return readFromStream(stream, terrain.terrainAttribute, terrain.physicalParameters) && terrain.isValid();
}

bool readFromStream(sf::InputStream& stream, TileSet::SingleObject& object)
{
    if (!readFromStream(stream, object.objectAttribute, object.objectMode)) return false;
    if (!TileSet::isAttributeValid(object.objectAttribute)) return false;

    switch (object.objectMode)
    {
        case TileSet::SingleObject::ObjectMode::TileMultiple:
            object.points.resize(1);
            return readFromStream(stream, object.points[0], object.radius);
        case TileSet::SingleObject::ObjectMode::Circle:
            return readFromStream(stream, object.radius);
        case TileSet::SingleObject::ObjectMode::Segment:
            object.points.resize(2);
            return readFromStream(stream, object.points[0], object.points[1], object.radius);
        case TileSet::SingleObject::ObjectMode::Polygon:
            return readFromStream(stream, object.points, object.radius);
        default: return false;
    }
}

bool readFromStream(sf::InputStream& stream, TileSet& tileSet)
{
    if (!readFromStream(stream, tileSet.textureName, tileSet.terrains, tileSet.singleObjects)) return false;

    size_t size;
    if (!readFromStream(stream, VarLength(size))) return false;
    tileSet.tileIdentities.resize(size);

    for (auto& id : tileSet.tileIdentities)
        if (!readFromStream(stream, id.type, VarLength(id.id))) return false;
    
    return true;
}

TileSet::Attribute TileSet::getTileAttribute(size_t id) const
{
    if (id >= tileIdentities.size()) return Attribute::None;
    if (isTerrain(tileIdentities[id].type))
        return terrains[tileIdentities[id].id].terrainAttribute;
    else if (isSemiTerrain(tileIdentities[id].type))
        return getSemiTerrainAttribute(tileIdentities[id].id);
    else if (isSingleObject(tileIdentities[id].type))
        return singleObjects[tileIdentities[id].id].objectAttribute;
    else return Attribute::None;
}
