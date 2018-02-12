#pragma once

#include <cstdint>

struct AudioSettings
{
    uint8_t musicVolume;
    uint8_t soundVolume;
};

bool readFromStream(sf::InputStream &stream, AudioSettings& settings);
bool writeToStream(OutputStream& stream, const AudioSettings& settings);
