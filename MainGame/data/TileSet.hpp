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

#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <InputStream.hpp>
#include <glm/glm.hpp>

template <typename T>
static constexpr bool isContained(T) { return false; }

template <typename T, typename... Ts>
static constexpr bool isContained(T comp, T first, Ts... next)
{
    return comp == first || isContained(comp, next...);
}

struct TileSet final
{
    enum class Attribute : uint8_t
    {
        None, Solid, NoWalljump, Spike, Crumbling,
        LeftSolid, RightSolid, UpSolid, DownSolid,
        LeftNoWalljump, RightNoWalljump, NumAttrs
    };
    static bool isAttributeValid(Attribute attr) { return (uint8_t)attr < (uint8_t)Attribute::NumAttrs; }

    static bool isNoWalljump(Attribute attr)
    {
        return isContained(attr, Attribute::NoWalljump, Attribute::LeftNoWalljump, Attribute::RightNoWalljump);
    }

#pragma pack(push, 1)
    struct PhysicalParameters final
    {
        int16_t upperOffset, lowerOffset;
        int16_t leftOffset, rightOffset;
        int16_t cornerRadius;

        PhysicalParameters() : upperOffset(0), lowerOffset(0), leftOffset(0), rightOffset(0), cornerRadius(1) {}
    };
#pragma pack(pop)

    static_assert(sizeof(PhysicalParameters) == 5 * sizeof(int16_t), "Compiler did not pack PhysicalParameters correctly!");

    struct Terrain final
    {
        enum TileCategory : uint8_t { UpperLeft, Up, UpperRight, Left, Center, Right, LowerLeft, Down, LowerRight };
        
        PhysicalParameters physicalParameters;
        Attribute terrainAttribute;

        bool isValid() const
        {
            return isContained(terrainAttribute, Attribute::Solid, Attribute::NoWalljump);
        }
    };

    struct SingleObject final
    {
        enum class ShapeType : uint8_t { Tile, Circle, Segment, Polygon };
        struct Shape final
        {
            ShapeType type;
            int16_t radius; // multifunctional
            std::vector<glm::i16vec2> points; // also multifunctional
        };

        Attribute objectAttribute;
        float waitTime, crumbleTime;
        size_t crumblePieceSize;
        std::vector<Shape> shapes;
    };

    enum class TileType : uint8_t
    {
        None, TerrainUpperLeft, TerrainUp, TerrainUpperRight, TerrainLeft,
        TerrainCenter, TerrainRight, TerrainLowerLeft, TerrainDown, TerrainLowerRight,
        SemiTerrain1, SemiTerrain2, SemiTerrain3, SingleObjectTile
    };
    
    static bool isTerrain(TileType type)
    {
        return isContained(type, TileType::TerrainUpperLeft, TileType::TerrainUp,
            TileType::TerrainUpperRight, TileType::TerrainLeft, TileType::TerrainCenter,
            TileType::TerrainRight, TileType::TerrainLowerLeft, TileType::TerrainDown,
            TileType::TerrainLowerRight);
    }

    static bool isSemiTerrain(TileType type)
    {
        return isContained(type, TileType::SemiTerrain1, TileType::SemiTerrain2, TileType::SemiTerrain3);
    }

    static bool isSingleObject(TileType type)
    {
        return type == TileType::SingleObjectTile;
    }

    static bool isIdVerticalSemiTerrain(size_t id)
    {
        return isContained(id, (size_t)2, (size_t)3);
    }

    static bool isIdHorizontalSemiTerrain(size_t id)
    {
        return isContained(id, (size_t)0, (size_t)1, (size_t)4, (size_t)5);
    }

    static Attribute getSemiTerrainAttribute(size_t id)
    {
        constexpr Attribute attrs[] =
        {
            Attribute::LeftSolid, Attribute::RightSolid, Attribute::UpSolid, Attribute::DownSolid,
            Attribute::LeftNoWalljump, Attribute::RightNoWalljump
        };
        return attrs[id];
    }

    struct TileIdentity { TileType type; size_t id; };

    static bool refersToSame(const TileIdentity& t1, const TileIdentity& t2)
    {
        if (t1.type == TileSet::TileType::None) return t2.type == TileSet::TileType::None;
        if (t2.type == TileSet::TileType::None) return false;

        if (!(TileSet::isTerrain(t1.type) && TileSet::isTerrain(t2.type)) &&
            !(TileSet::isSemiTerrain(t1.type) && TileSet::isSemiTerrain(t2.type)) &&
            !(TileSet::isSingleObject(t1.type) && TileSet::isSingleObject(t2.type)))
            return false;

        return t1.id == t2.id;
    }
    
    std::string textureName;
    std::vector<Terrain> terrains;
    std::vector<SingleObject> singleObjects;

    std::vector<TileIdentity> tileIdentities;

    Attribute getTileAttribute(size_t id) const;

    static constexpr auto ReadMagic = "TSET";
};

bool readFromStream(InputStream &stream, TileSet& tileSet);
