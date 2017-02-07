#pragma once

#include <cstdint>
#include <chrono>

constexpr size_t ScreenWidth = 768;
constexpr size_t ScreenHeight = 576;

constexpr size_t PlayfieldWidth = 576;
constexpr size_t PlayfieldHeight = 576;

constexpr size_t DefaultTileSize = 32;

constexpr auto UpdateFrequency = std::chrono::milliseconds(16);
