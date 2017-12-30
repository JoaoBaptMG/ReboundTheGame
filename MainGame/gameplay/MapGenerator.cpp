#include "MapGenerator.hpp"

#include <algorithm>

#include "defaults.hpp"
#include "data/TileSet.hpp"
#include "resources/ResourceManager.hpp"
#include <assert.hpp>
#include <queue>

#ifdef GENERATE_MAPS_IF_EMPTY
util::grid<bool> generateMap(const RoomData& data, ResourceManager& manager)
{
    auto tileSet = manager.load<TileSet>(data.tilesetName + ".ts");
    util::grid<bool> map(data.mainLayer.width(), data.mainLayer.height());
    
    std::transform(data.mainLayer.begin(), data.mainLayer.end(), map.begin(),
    [&] (uint8_t t) { return tileSet->getTileAttribute(t) != TileSet::Attribute::None; });
    
    return map;
}
#endif

std::unique_ptr<uint8_t[]> getTextureData(const util::grid<bool>& grid)
{
    std::unique_ptr<uint8_t[]> pixels{new uint8_t[4*grid.width()*grid.height()]};
    uint8_t* cur = pixels.get();
    
    for (bool v : grid)
    {
        *cur++ = 255;
        *cur++ = 255;
        *cur++ = 255;
        *cur++ = v ? 255 : 64;
    }
    
    return pixels;
}

#ifdef GENERATE_MAPS_IF_EMPTY
void generateAllMapsForLevel(LevelData& level, ResourceManager& manager)
{
    level.roomMaps.resize(level.roomResourceNames.size());
    
    std::queue<size_t> roomsToProcess;
    roomsToProcess.push(level.startingRoom);
    
    while (!roomsToProcess.empty())
    {
        size_t cur = roomsToProcess.front();
        roomsToProcess.pop();
        
        if (!level.roomMaps.at(cur).map.empty()) continue;
        
        auto data = manager.load<RoomData>(level.roomResourceNames.at(cur) + ".map");
        level.roomMaps.at(cur).map = generateMap(*data, manager);
        
        for (const auto& warp : data->warps)
        {
            size_t other = warp.roomId;
            roomsToProcess.push(other);
            
            if (level.roomMaps.at(other).map.empty())
            {
                auto otherData = manager.load<RoomData>(level.roomResourceNames.at(other) + ".map");
                auto delta = otherData->warps.at(warp.warpId).c1 - warp.c1;
                
                switch (warp.warpDir)
                {
                    case WarpData::Dir::Right:
                        level.roomMaps.at(other).x = level.roomMaps.at(cur).x + data->mainLayer.width();
                        level.roomMaps.at(other).y = level.roomMaps.at(cur).y - (size_t)(delta/DefaultTileSize);
                        break;
                    case WarpData::Dir::Down:
                        level.roomMaps.at(other).y = level.roomMaps.at(cur).y + data->mainLayer.height();
                        level.roomMaps.at(other).x = level.roomMaps.at(cur).x - (size_t)(delta/DefaultTileSize);
                        break;
                    case WarpData::Dir::Left:
                        level.roomMaps.at(other).x = level.roomMaps.at(cur).x - otherData->mainLayer.width();
                        level.roomMaps.at(other).y = level.roomMaps.at(cur).y - (size_t)(delta/DefaultTileSize);
                        break;
                    case WarpData::Dir::Up:
                        level.roomMaps.at(other).y = level.roomMaps.at(cur).y - otherData->mainLayer.height();
                        level.roomMaps.at(other).x = level.roomMaps.at(cur).x - (size_t)(delta/DefaultTileSize);
                        break;
                }
            }
        }
    }
}
#endif
