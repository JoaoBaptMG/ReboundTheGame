//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

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

bool readFromStream(sf::InputStream& stream, TileSet::SingleObject::Shape& shape)
{
    if (!readFromStream(stream, shape.type)) return false;
    
    switch (shape.type)
    {
        case TileSet::SingleObject::ShapeType::Tile:
        case TileSet::SingleObject::ShapeType::Circle:
            return readFromStream(stream, shape.radius);
        case TileSet::SingleObject::ShapeType::Segment:
            shape.points.resize(2);
            return readFromStream(stream, shape.points[0], shape.points[1], shape.radius);
        case TileSet::SingleObject::ShapeType::Polygon:
            return readFromStream(stream, shape.points, shape.radius);
        default: return false;
    }
}

bool readFromStream(sf::InputStream& stream, TileSet::SingleObject& object)
{
    if (!readFromStream(stream, object.objectAttribute)) return false;
    if (object.objectAttribute == TileSet::Attribute::Crumbling &&
        !readFromStream(stream, object.waitTime, object.crumbleTime, varLength(object.crumblePieceSize))) return false;
    if (!readFromStream(stream, object.shapes)) return false;
    return TileSet::isAttributeValid(object.objectAttribute);
}

bool readFromStream(sf::InputStream& stream, TileSet& tileSet)
{
    if (!readFromStream(stream, tileSet.textureName, tileSet.terrains, tileSet.singleObjects)) return false;

    size_t size;
    if (!readFromStream(stream, varLength(size))) return false;
    tileSet.tileIdentities.resize(size);

    for (auto& id : tileSet.tileIdentities)
        if (!readFromStream(stream, id.type, varLength(id.id))) return false;
    
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
