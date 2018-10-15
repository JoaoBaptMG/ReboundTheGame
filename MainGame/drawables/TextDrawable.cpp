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

#include "TextDrawable.hpp"
#include <algorithm>
#include <utf8.h>
#include <cmath>
#include <predUtils.hpp>
#include "unicode/UnicodeUtils.hpp"

bool isGraphemeClusterBoundary(sf::Uint32 c1, sf::Uint32 c2)
{
    // TODO: add all remaining cases
    return !unicode::isGraphemeExtender(c2);
}

bool (*const WordWrappingAlgorithms[])(uint32_t,uint32_t) =
{
    // Normal
    [](uint32_t pc, uint32_t c) -> bool
    {
        return unicode::isCharacterWrappable(c) || unicode::isCharacterNewline(c);
    },

    // Japanese
    [](uint32_t pc, uint32_t c) -> bool
    {
        if (pc <= 127 && c <= 127)
            return unicode::isCharacterWrappable(c) || unicode::isCharacterNewline(c);

        return unicode::isJapaneseWrappable(pc, c);
    },

    // Simplified Chinese
    [](uint32_t pc, uint32_t c) -> bool
    {
        const std::string nextForbidden = u8"!%),.:;?]}¢°·’\"\"†‡›℃∶、。〃〆〕〗〞﹚﹜！＂％＇），．：；？！］｝～";
        const std::string prevForbidden = u8"$(£¥·‘\"〈《「『【〔〖〝﹙﹛＄（．［｛￡￥";

        if (nextForbidden.find(c) != std::string::npos) return false;
        if (prevForbidden.find(pc) != std::string::npos) return false;

        return true;
    },

    // Traditional Chinese
    [](uint32_t pc, uint32_t c) -> bool
    {
        const std::string nextForbidden = u8"!),.:;?]}¢·–— ’\"•\" 、。〆〞〕〉》」︰︱︲︳﹐﹑﹒﹓﹔﹕﹖﹘﹚﹜！），．：；？︶︸︺︼︾﹀﹂﹗］｜｝､";
        const std::string prevForbidden = u8"([{£¥‘\"‵〈《「『〔〝︴﹙﹛（｛︵︷︹︻︽︿﹁﹃﹏";

        if (nextForbidden.find(c) != std::string::npos) return false;
        if (prevForbidden.find(pc) != std::string::npos) return false;

        return true;
    },

    // Korean
    [](uint32_t pc, uint32_t c) -> bool
    {
        const std::string nextForbidden = u8"!%),.:;?]}¢°’\"†‡℃〆〈《「『〕！％），．：；？］｝";
        const std::string prevForbidden = u8"$([\\{£¥‘\"々〇〉》」〔＄（［｛｠￥￦ #";

        if (nextForbidden.find(c) != std::string::npos) return false;
        if (prevForbidden.find(pc) != std::string::npos) return false;

        return true;
    },
};

TextDrawable::TextDrawable(std::shared_ptr<FontHandler> fontHandler) : fontHandler(fontHandler), utf8String(),
    fontSize(30), defaultColor(sf::Color::White), defaultOutlineColor(sf::Color::Black),
    outlineThickness(0), wordWrappingWidth(0), wordAlignment(TextDrawable::Alignment::Direct),
    horizontalAnchor(TextDrawable::HorAnchor::Left), verticalAnchor(TextDrawable::VertAnchor::Top), rtl(false),
    needsUpdateGeometry(false), vertices(sf::PrimitiveType::Triangles),
    verticesOutline(sf::PrimitiveType::Triangles), bounds(), graphemeClusters()
{
    wordWrappingAlgorithm = WordWrappingAlgorithm::Normal;
}

inline static auto addGlyphQuad(sf::VertexArray& dest, sf::Vector2f position, sf::Color color, const sf::Glyph& glyph)
{
    const auto& bounds = glyph.bounds;
    sf::FloatRect texRect((float)glyph.textureRect.left, (float)glyph.textureRect.top,
        (float)glyph.textureRect.width, (float)glyph.textureRect.height);

    auto s = dest.getVertexCount();

    if (bounds == sf::FloatRect(0, 0, 0, 0))
        return std::make_pair(s, s);

    dest.append(sf::Vertex(position + sf::Vector2f(bounds.left, bounds.top),
        color, sf::Vector2f(texRect.left, texRect.top)));
    dest.append(sf::Vertex(position + sf::Vector2f(bounds.left + bounds.width, bounds.top),
        color, sf::Vector2f(texRect.left + texRect.width, texRect.top)));
    dest.append(sf::Vertex(position + sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height),
        color, sf::Vector2f(texRect.left + texRect.width, texRect.top + texRect.height)));
    dest.append(sf::Vertex(position + sf::Vector2f(bounds.left, bounds.top + bounds.height),
        color, sf::Vector2f(texRect.left, texRect.top + texRect.height)));
    dest.append(dest[s]);
    dest.append(dest[s+2]);

    return std::make_pair(s, s+6);
}

float TextDrawable::getLineSpacing() const
{
    return fontHandler->getFont().getLineSpacing(fontSize);
}

float TextDrawable::getHeightForLineNumber(size_t lines) const
{
    const auto& font = fontHandler->getFont();

    if (lines == 0) return 0;
    return font.getDescent(fontSize) - font.getAscent(fontSize) + (lines-1) * font.getLineSpacing(fontSize);
}

void TextDrawable::buildGeometry()
{
    if (!needsUpdateGeometry) return;

    vertices.clear();
    verticesOutline.clear();
    graphemeClusters.clear();

    if (fontHandler == nullptr) return;
    if (utf8String.empty()) return;

    if (!utf8::is_valid(utf8String.begin(), utf8String.end()))
#ifdef NDEBUG
        return;
#else
        utf8String = u8"!!!INVALID UTF-8 STRING PASSED TO TextDrawable!!!";
#endif

    sf::Font& font = fontHandler->getFont();
    HarfBuzzWrapper& wrapper = fontHandler->getHBWrapper();
    wrapper.setFontSize(fontSize);

    bool outline = outlineThickness != 0;

    const char* curPos = utf8String.data();
    const char* endPos = utf8String.data() + utf8String.size();

    struct Word
    {
        size_t vertexBegin, vertexEnd, graphemeClusterEnd;
        uint32_t nextCharacter;
        float wordWidth;
    };

    std::vector<Word> words;

    const char* nextWordPos;
    do
    {
        nextWordPos = curPos;
        uint32_t pc = 0;
        while (nextWordPos != endPos)
        {
            uint32_t c = utf8::unchecked::next(nextWordPos);
            if ((pc != 0 && WordWrappingAlgorithms[(size_t)wordWrappingAlgorithm](pc, c))
                || unicode::isCharacterWordJoiner(c))
            {
                utf8::unchecked::prior(nextWordPos);
                break;
            }
            pc = c;
        }

        size_t prevCluster = (size_t)-1;
        size_t curWordBegin = vertices.getVertexCount();
        size_t curGraphemeBegin = graphemeClusters.size();
        sf::Vector2f position;

        auto prop = wrapper.shape(curPos, nextWordPos - curPos);
        for (const auto& glyphData : prop.glyphs)
        {
            uint32_t c = glyphData.codepoint;
            size_t cluster = glyphData.cluster;

            if (outline)
            {
                auto tempPos = position + glyphData.offset - sf::Vector2f(outlineThickness, outlineThickness);
                const auto& glyph = font.getGlyphId(c, fontSize, false, outlineThickness);
                addGlyphQuad(verticesOutline, tempPos, defaultOutlineColor, glyph);
            }

            const auto& glyph = font.getGlyphId(c, fontSize, false);
            auto p = addGlyphQuad(vertices, position + glyphData.offset, defaultColor, glyph);

            position += glyphData.advance;

            if (prevCluster == cluster || p.first == p.second)
            {
                if (!graphemeClusters.empty())
                    graphemeClusters.back().vertexEnd = p.second;
            }
            else graphemeClusters.push_back({ p.first, p.second, (size_t)(curPos - utf8String.data()) });

            prevCluster = cluster;
        }

        uint32_t c = 0;
        if (wordWrappingAlgorithm == WordWrappingAlgorithm::Normal && nextWordPos != endPos)
            c = utf8::unchecked::next(nextWordPos);

        words.push_back({ curWordBegin, vertices.getVertexCount(), graphemeClusters.size(), c, position.x });

        if (prop.isRTL)
        {
            std::reverse(graphemeClusters.data() + curGraphemeBegin,
                         graphemeClusters.data() + graphemeClusters.size());
        }

        curPos = nextWordPos;
    } while (nextWordPos != endPos);

    {
        struct Line
        {
            size_t vertexBegin, vertexEnd, graphemeClusterEnd;
            float lineWidth;
        };

        std::vector<Line> lines;

        sf::Vector2f curPos;

        if (wordWrappingWidth > 0)
        {
            bounds.left = rtl ? -wordWrappingWidth : 0;
            bounds.width = wordWrappingWidth;
        }
        else
        {
            bounds.left = 0;
            bounds.width = 0;
        }

        bounds.top = font.getAscent(fontSize);

        const Word* lastBegin = &words.front();
        float curLineWidth = 0;
        for (size_t i = 0; i < words.size(); i++)
        {
            const auto& word = words[i];

            if (wordWrappingWidth > 0)
            {
                float joinedWordWidth = word.wordWidth;
                for (size_t k = 0; k < words.size() && unicode::isCharacterWordJoiner(words[i+k].nextCharacter); k++)
                    joinedWordWidth += words[i+k+1].wordWidth;

                if (rtl ? curPos.x - joinedWordWidth < -wordWrappingWidth
                        : curPos.x + joinedWordWidth >  wordWrappingWidth)
                {
                    if (i > 0) lines.push_back({ lastBegin->vertexBegin, words[i-1].vertexEnd,
                                                 words[i-1].graphemeClusterEnd, curLineWidth });
                    lastBegin = &word;

                    curPos.x = 0;
                    curPos.y += font.getLineSpacing(fontSize);
                }
            }

            if (rtl) curPos.x -= word.wordWidth;

            for (size_t i = word.vertexBegin; i < word.vertexEnd; i++)
            {
                vertices[i].position += curPos;
                if (outline) verticesOutline[i].position += curPos;

                //bounds = rectUnionWithPoint(bounds, vertices[i].position);
            }

            if (!rtl) curPos.x += word.wordWidth;
            bounds = rectUnionWithLineX(bounds, curPos.x);
            curLineWidth = curPos.x;

            if (word.nextCharacter)
            {
                if (unicode::isCharacterNewline(word.nextCharacter))
                {
                    lines.push_back({ lastBegin->vertexBegin, word.vertexEnd, word.graphemeClusterEnd, curLineWidth });
                    lastBegin = &words[i+1];

                    curPos.x = 0;
                    curPos.y += font.getLineSpacing(fontSize);
                }
                else if (!unicode::isCharacterZeroWidth(word.nextCharacter))
                {
                    const auto& glyph = font.getGlyph(word.nextCharacter, fontSize, false);
                    if (rtl) curPos.x -= glyph.advance;
                    else curPos.x += glyph.advance;
                }
            }
        }

        bounds = rectUnionWithLineX(bounds, curLineWidth);
        lines.push_back({ lastBegin->vertexBegin, words.back().vertexEnd, words.back().graphemeClusterEnd, curLineWidth });
        bounds.height = (lines.size() - 1) * font.getLineSpacing(fontSize) + font.getDescent(fontSize) - bounds.top;

        lineBoundaries.clear();
        for (const auto& line : lines)
        {
            float lineDisplacement = (rtl ? -bounds.width : bounds.width) - line.lineWidth;
            switch (wordAlignment)
            {
                case Alignment::Direct: lineDisplacement = 0; break;
                case Alignment::Center: lineDisplacement = floorf(lineDisplacement/2); break;
                case Alignment::Inverse: break;
            }

            for (size_t i = line.vertexBegin; i < line.vertexEnd; i++)
            {
                vertices[i].position.x += lineDisplacement;
                if (outline) verticesOutline[i].position.x += lineDisplacement;
            }

            lineBoundaries.push_back(line.graphemeClusterEnd);
        }
    }

    sf::Vector2f revert;
    switch (horizontalAnchor)
    {
        case HorAnchor::Left: revert.x = bounds.left; break;
        case HorAnchor::Center: revert.x = floorf(bounds.left + bounds.width/2); break;
        case HorAnchor::Right: revert.x = bounds.left + bounds.width; break;
    }
    switch (verticalAnchor)
    {
        case VertAnchor::Top: revert.y = bounds.top; break;
        case VertAnchor::Center: revert.y = floorf(bounds.top + bounds.height/2); break;
        case VertAnchor::Bottom: revert.y = bounds.top + bounds.height; break;
        case VertAnchor::Baseline: break;
    }

    bounds.left -= revert.x;
    bounds.top -= revert.y;

    for (size_t i = 0; i < vertices.getVertexCount(); i++)
    {
        vertices[i].position -= revert;
        if (outline) verticesOutline[i].position -= revert;
    }

    needsUpdateGeometry = false;
}

TextDrawable::GraphemeRange TextDrawable::getGraphemeClusterInterval(size_t begin, size_t end, bool outline)
{
    if (end > graphemeClusters.size()) end = graphemeClusters.size();
    if (begin >= end) return { nullptr, nullptr };
    if (outline && verticesOutline.getVertexCount() == 0) return { nullptr, nullptr };

    const auto& clusterb = graphemeClusters.at(begin);
    const auto& clustere = graphemeClusters.at(end-1);
    auto vlist = outline ? &verticesOutline[0] : &vertices[0];
    return { vlist+clusterb.vertexBegin, vlist+clustere.vertexEnd };
}

TextDrawable::GraphemeRange TextDrawable::getGraphemeCluster(size_t index, bool outline)
{
    if (index >= graphemeClusters.size()) return { nullptr, nullptr };
    if (outline && verticesOutline.getVertexCount() == 0) return { nullptr, nullptr };

    const auto& cluster = graphemeClusters.at(index);
    auto vlist = outline ? &verticesOutline[0] : &vertices[0];
    return { vlist+cluster.vertexBegin, vlist+cluster.vertexEnd };
}

size_t TextDrawable::getGraphemeClusterByteLocation(size_t index) const
{
    if (index >= graphemeClusters.size()) return utf8String.size();
    return graphemeClusters.at(index).byteLocation;
}

size_t TextDrawable::getGraphemeClusterLine(size_t index) const
{
    if (index >= graphemeClusters.size()) return lineBoundaries.size();
    auto it = std::upper_bound(lineBoundaries.begin(), lineBoundaries.end(), index);
    return (size_t)(it - lineBoundaries.begin());
}

size_t TextDrawable::getNumberOfGraphemeClusters() const
{
    return graphemeClusters.size();
}

size_t TextDrawable::getLineBoundary(size_t index) const
{
    if (lineBoundaries.empty()) return 0;
    if (index >= lineBoundaries.size())
        index = lineBoundaries.size() - 1;
    return lineBoundaries.at(index);
}

size_t TextDrawable::getNumberOfLines() const
{
    return lineBoundaries.size();
}

TextDrawable::GraphemeRange TextDrawable::getAllVertices(bool outline)
{
    if (graphemeClusters.empty()) return { nullptr, nullptr };
    if (outline && verticesOutline.getVertexCount() == 0) return { nullptr, nullptr };

    auto vlist = outline ? &verticesOutline[0] : &vertices[0];
	auto size = outline ? verticesOutline.getVertexCount() : vertices.getVertexCount();
    return { vlist, vlist+size };
}

void TextDrawable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!needsUpdateGeometry)
        states.texture = &fontHandler->getFont().getTexture(fontSize);

    target.draw(verticesOutline, states);
    target.draw(vertices, states);
}



