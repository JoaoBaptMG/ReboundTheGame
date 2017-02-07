#pragma once

#include "utility/grid.hpp"
#include "utility/non_copyable_movable.hpp"
#include "resources/Memory.hpp"

#include <vector>
#include <SFML/System.hpp>

struct GameObjectDescriptor
{
    std::string klass;
    Memory parameters;
};

struct RoomData final : util::non_copyable
{
public:
    util::grid<uint8_t> mainLayer;
    std::string textureName;
    std::vector<GameObjectDescriptor> gameObjectDescriptors;

    RoomData() : mainLayer(), textureName() {}
    RoomData(RoomData&& other) noexcept : RoomData() { swap(*this, other); }
    ~RoomData() {}

    RoomData& operator=(RoomData&& other) noexcept { swap(*this, other); return *this; }

    friend void swap(RoomData& r1, RoomData &r2)
    {
        using std::swap;
        swap(r1.mainLayer, r2.mainLayer);
        swap(r1.textureName, r2.textureName);
    }

    bool loadFromStream(sf::InputStream &stream);
};

