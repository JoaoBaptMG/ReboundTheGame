#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include <grid.hpp>

#include "data/RoomData.hpp"
#include "data/LevelData.hpp"

class ResourceManager;

std::unique_ptr<uint8_t[]> getTextureData(const util::grid<bool>& grid);

#ifdef GENERATE_MAPS_IF_EMPTY
util::grid<bool> generateMap(const RoomData& data, ResourceManager& manager);
void generateAllMapsForLevel(LevelData& level, ResourceManager& manager);
#endif
