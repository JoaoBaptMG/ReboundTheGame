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

#include "HarfBuzzWrapper.hpp"
#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ot.h>

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
            glm::vec2(positions[i].x_offset, -positions[i].y_offset),
            glm::vec2(positions[i].x_advance, -positions[i].y_advance) });
    }
    
    hb_segment_properties_t properties;
    hb_buffer_get_segment_properties(buffer, &properties);
    
    hb_buffer_destroy(buffer);
    return { HB_DIRECTION_IS_BACKWARD(properties.direction), glyphs };
}
