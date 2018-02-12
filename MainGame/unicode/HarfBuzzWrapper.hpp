#pragma once

#include <SFML/System.hpp>
#include <cstdint>
#include <string>
#include <vector>
#include <hb.h>

struct StringProperties
{
    struct GlyphData
    {
        uint32_t codepoint;
        size_t cluster;
        sf::Vector2f offset, advance;
    };
    
    bool isRTL;
    std::vector<GlyphData> glyphs;
};

class HarfBuzzWrapper final
{
    hb_blob_t* blob;
    hb_face_t* face;
    hb_font_t* font;
    
public:
    HarfBuzzWrapper(const char* data, size_t size);
    ~HarfBuzzWrapper();
    
    void setFontSize(size_t size);
    StringProperties shape(const char* string, size_t strSize);
};
