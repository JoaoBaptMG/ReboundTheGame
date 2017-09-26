#pragma once

#include <grid.hpp>
#include <generic_ptrs.hpp>
#include <non_copyable_movable.hpp>
#include "resources/Memory.hpp"

#include <vector>
#include <SFML/System.hpp>

template <size_t size> struct Print;

struct GameObjectDescriptor final
{
    std::string klass;
    util::generic_shared_ptr parameters;
};

#pragma pack(push, 1)
struct WarpData final
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

struct RoomData final
{
    util::grid<uint8_t> mainLayer;
    std::string tilesetName;
    std::vector<GameObjectDescriptor> gameObjectDescriptors;
    std::vector<WarpData> warps;

    static constexpr auto ReadMagic = "ROOM";
};

bool readFromStream(sf::InputStream &stream, RoomData& room);

