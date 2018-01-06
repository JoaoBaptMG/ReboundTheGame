#include "TextDrawable.hpp"
#include <algorithm>
#include <utf8.h>
#include <cmath>
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
        const std::string prevForbidden = u8"$([\{£¥‘\"々〇〉》」〔＄（［｛｠￥￦ #";
        
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
    
    prevStringLength = utf8String.size();
    
    sf::Font& font = fontHandler->getFont();
    HarfBuzzWrapper& wrapper = fontHandler->getHBWrapper();
    wrapper.setFontSize(fontSize);
    
    bool outline = outlineThickness != 0;
    
    const char* curPos = utf8String.data();
    const char* endPos = utf8String.data() + utf8String.size();
    
    const char* nextWordPos;
    
    struct Word
    {
        size_t vertexBegin, vertexEnd;
        uint32_t nextCharacter;
        float wordWidth;
    };
    
    std::vector<Word> words;
    
    do
    {
        nextWordPos = curPos;
        uint32_t pc = 0;
        while (nextWordPos != endPos)
        {
            uint32_t c = utf8::unchecked::next(nextWordPos);
            if (pc != 0 && WordWrappingAlgorithms[(size_t)wordWrappingAlgorithm](pc, c))
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
            else graphemeClusters.push_back({ p.first, p.second });
            
            prevCluster = cluster;
        }
        
        uint32_t c = 0;
        if (wordWrappingAlgorithm == WordWrappingAlgorithm::Normal && nextWordPos != endPos)
            c = utf8::unchecked::next(nextWordPos);
        words.push_back({ curWordBegin, vertices.getVertexCount(), c, position.x });
        
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
            size_t vertexBegin, vertexEnd;
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
            bounds.width = NAN;
        }
        
        bounds.top = font.getAscent(fontSize);
        
        const Word* lastBegin = &words.front();
        float curLineWidth = 0;
        for (size_t i = 0; i < words.size(); i++)
        {
            const auto& word = words[i];
            
            if (wordWrappingWidth > 0)
            {
                if (rtl ? curPos.x - word.wordWidth < -wordWrappingWidth
                        : curPos.x + word.wordWidth >  wordWrappingWidth)
                {
                    if (i > 0) lines.push_back({ lastBegin->vertexBegin, words[i-1].vertexEnd, curLineWidth });
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
                    lines.push_back({ lastBegin->vertexBegin, word.vertexEnd, curLineWidth });
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
        lines.push_back({ lastBegin->vertexBegin, words.back().vertexEnd, curLineWidth });
        bounds.height = (lines.size() - 1) * font.getLineSpacing(fontSize) + font.getDescent(fontSize) - bounds.top;
        
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

TextDrawable::GraphemeRange TextDrawable::getGraphemeCluster(size_t index, bool outline)
{
    if (index >= graphemeClusters.size()) return { nullptr, nullptr };
    if (outline && verticesOutline.getVertexCount() == 0) return { nullptr, nullptr };
    
    const auto& cluster = graphemeClusters.at(index);
    auto& vlist = outline ? verticesOutline : vertices;
    return { &vlist[cluster.vertexBegin], &vlist[cluster.vertexEnd] };
}

size_t TextDrawable::getNumberOfGraphemeClusters() const
{
    return graphemeClusters.size();
}

void TextDrawable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!needsUpdateGeometry)
        states.texture = &fontHandler->getFont().getTexture(fontSize);
        
    target.draw(verticesOutline, states);
    target.draw(vertices, states);
}



