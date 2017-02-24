#pragma once

#include "utility/grid.hpp"
#include "utility/non_copyable_movable.hpp"
#include "resources/Memory.hpp"

#include <vector>
#include <SFML/System.hpp>

template <size_t size> struct Print;

struct GameObjectDescriptor
{
    std::string klass;
    Memory parameters;
};

#pragma pack(push, 1)
struct WarpData
{
    int16_t c1, c2; // y1, y2 if a vertical warp, x1, x2 if horizontal
    uint16_t roomId, warpId:14; // room ID on level, warp ID on room

    enum class Dir : uint16_t
    {
        Right, Down, Left, Up
    } warpDir:2;

    static inline Dir oppositeWarpDir(Dir dir)
    {
        switch (dir)
        {
            case Dir::Right: return Dir::Left;
            case Dir::Down: return Dir::Up;
            case Dir::Left: return Dir::Right;
            case Dir::Up: return Dir::Down;
        }
    }
};
#pragma pack(pop)
static_assert(sizeof(WarpData) == 8*sizeof(char), "WarpData was not correctly packed by the compiler!");

struct RoomData final : util::non_copyable
{
public:
    util::grid<uint8_t> mainLayer;
    std::string textureName;
    std::vector<GameObjectDescriptor> gameObjectDescriptors;
    std::vector<WarpData> warps;

    RoomData() : mainLayer(), textureName() {}
    RoomData(RoomData&& other) noexcept : RoomData() { swap(*this, other); }
    ~RoomData() {}

    RoomData& operator=(RoomData&& other) noexcept { swap(*this, other); return *this; }

    friend void swap(RoomData& r1, RoomData &r2) noexcept
    {
        using std::swap;
        swap(r1.mainLayer, r2.mainLayer);
        swap(r1.textureName, r2.textureName);
        swap(r1.gameObjectDescriptors, r2.gameObjectDescriptors);
        swap(r1.warps, r2.warps);
    }

    bool loadFromStream(sf::InputStream &stream);
};

bool readFromStream(sf::InputStream &stream, RoomData& room);

