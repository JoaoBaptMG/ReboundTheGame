#include "HarfBuzzWrapper.hpp"
#include <hb.h>
#include <hb-ot.h>

HarfBuzzWrapper::HarfBuzzWrapper(const char* data, size_t size)
{
    blob = hb_blob_create(data, size, HB_MEMORY_MODE_READONLY, nullptr, nullptr);
    face = hb_face_create(blob, 0);
    font = hb_font_create(face);
    hb_ot_font_set_funcs(font);
}

HarfBuzzWrapper::~HarfBuzzWrapper()
{
    if (font) hb_font_destroy(font);
    if (face) hb_face_destroy(face);
    if (blob) hb_blob_destroy(blob);
}

void HarfBuzzWrapper::setFontSize(size_t size)
{
    hb_font_set_scale(font, size, size);
}

StringProperties HarfBuzzWrapper::shape(const char* string, size_t strSize)
{
    hb_buffer_t* buffer = hb_buffer_create();
    hb_buffer_add_utf8(buffer, string, strSize, 0, strSize);
    hb_buffer_guess_segment_properties(buffer);
    hb_buffer_set_cluster_level(buffer, HB_BUFFER_CLUSTER_LEVEL_MONOTONE_GRAPHEMES);
    
    hb_shape(font, buffer, nullptr, 0);
    
    unsigned int glyphCount;
    hb_glyph_info_t *info = hb_buffer_get_glyph_infos(buffer, &glyphCount);
    hb_glyph_position_t *positions = hb_buffer_get_glyph_positions(buffer, &glyphCount);
    
    std::vector<StringProperties::GlyphData> glyphs;
    
    size_t prevCluster = (size_t)-1;
    size_t clusterIndex = (size_t)-1;
    glyphs.reserve(glyphCount);
    for (size_t i = 0; i < glyphCount; i++)
    {
        if (prevCluster != info[i].cluster)
        {
            prevCluster = info[i].cluster;
            clusterIndex++;
        }
        
        glyphs.push_back({ info[i].codepoint, clusterIndex,
            sf::Vector2f(positions[i].x_offset, -positions[i].y_offset),
            sf::Vector2f(positions[i].x_advance, -positions[i].y_advance) });
    }
    
    hb_segment_properties_t properties;
    hb_buffer_get_segment_properties(buffer, &properties);
    
    hb_buffer_destroy(buffer);
    return { HB_DIRECTION_IS_BACKWARD(properties.direction), glyphs };
}
