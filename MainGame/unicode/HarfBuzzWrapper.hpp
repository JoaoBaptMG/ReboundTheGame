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


#pragma once

#include <glm/glm.hpp>
#include <SFML/System.hpp>
#include <cstdint>
#include <string>
#include <vector>
#include <harfbuzz/hb.h>

struct StringProperties
{
    struct GlyphData
    {
        uint32_t codepoint;
        size_t cluster;
        glm::vec2 offset, advance;
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
