#pragma once

#include <string>
#include <vector>
#include <SFML/System.hpp>

struct TileSet final
{
    enum class Mode : uint8_t { Normal, Solid, Water };
    
    std::string textureName;

#pragma pack(push, 1)
    struct
    {
        int16_t upperOffset, lowerOffset;
        int16_t leftOffset, rightOffset;
        int16_t cornerRadius;
    } physicalParameters;
#pragma pack(pop)

    static_assert(sizeof(physicalParameters) == 5 * sizeof(int16_t), "Compiler did not pack physicalParameters correctly!");
    
    std::vector<Mode> tileModes;

    bool loadFromStream(sf::InputStream& stream);
};

bool readFromStream(sf::InputStream &stream, TileSet& tileSet);