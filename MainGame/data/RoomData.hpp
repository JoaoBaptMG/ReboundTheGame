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

#include <grid.hpp>
#include <generic_ptrs.hpp>
#include <non_copyable_movable.hpp>
#include "resources/Memory.hpp"

#include <vector>
#include <InputStream.hpp>

template <size_t size> struct Print;

struct GameObjectDescriptor final
{
    std::string klass, name;
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

bool readFromStream(InputStream &stream, RoomData& room);

