#pragma once

#include <cstdint>

struct VideoSettings
{
    uint8_t fullscreen;
    uint8_t vsyncEnabled;
};

bool readFromStream(sf::InputStream &stream, VideoSettings& settings);
bool writeToStream(OutputStream& stream, const VideoSettings& settings);
