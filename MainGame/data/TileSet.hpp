#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <SFML/System.hpp>

template <typename T>
constexpr bool isContained(T comp) { return false; }

template <typename T, typename... Ts>
constexpr bool isContained(T comp, T first, Ts... next)
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

#pragma pack(push, 1)
    struct PhysicalParameters
    {
        int16_t upperOffset, lowerOffset;
        int16_t leftOffset, rightOffset;
        int16_t cornerRadius;

        PhysicalParameters() : upperOffset(0), lowerOffset(0), leftOffset(0), rightOffset(0), cornerRadius(1) {}
    };
#pragma pack(pop)

    static_assert(sizeof(PhysicalParameters) == 5 * sizeof(int16_t), "Compiler did not pack PhysicalParameters correctly!");

    struct Terrain
    {
        enum TileCategory : uint8_t { UpperLeft, Up, UpperRight, Left, Center, Right, LowerLeft, Down, LowerRight };
        
        PhysicalParameters physicalParameters;
        Attribute terrainAttribute;

        bool isValid() const
        {
            return isContained(terrainAttribute, Attribute::Solid, Attribute::NoWalljump);
        }
    };

    struct SingleObject
    {
        enum class ObjectMode : uint8_t { TileMultiple, Circle, Segment, Polygon };

        Attribute objectAttribute;
        ObjectMode objectMode;
        int16_t radius; // multifunctional
        std::vector<sf::Vector2<int16_t>> points; // also multifunctional
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

bool readFromStream(sf::InputStream &stream, TileSet& tileSet);
