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

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <utility>
#include <string>
#include <map>
#include <functional>
#include <rectUtils.hpp>
#include "resources/FontHandler.hpp"

class TextDrawable final : public sf::Drawable
{
public:
    enum class Alignment { Direct, Center, Inverse };
    
    enum class HorAnchor { Left, Center, Right };
    enum class VertAnchor { Top, Center, Bottom, Baseline };
    
    enum class WordWrappingAlgorithm { Normal, Japanese, Chinese, Korean };
    
    struct GraphemeRange
    {
        sf::Vertex *v1, *v2;
        auto begin() const { return v1; }
        auto end() const { return v2; }
    };
    
private:
    struct GraphemeClusterData
    {
        size_t vertexBegin, vertexEnd;
        size_t byteLocation;
    };

    std::shared_ptr<FontHandler> fontHandler;
    
    std::string utf8String = "";
    size_t fontSize = 30;
    sf::Color defaultColor, defaultOutlineColor;
    float outlineThickness = 0;
    
    float wordWrappingWidth = 0;
    WordWrappingAlgorithm wordWrappingAlgorithm;
    Alignment wordAlignment;
    HorAnchor horizontalAnchor;
    VertAnchor verticalAnchor;
    
    bool rtl = false;
    bool needsUpdateGeometry = false;
    sf::VertexArray vertices, verticesOutline;
    
    sf::FloatRect bounds;

    std::vector<GraphemeClusterData> graphemeClusters;
    std::vector<size_t> lineBoundaries;
    
public:
    explicit TextDrawable(std::shared_ptr<FontHandler> fontHandler = nullptr);
    ~TextDrawable() {}
    
    auto getFontHandler() const { return fontHandler; }
    void setFontHandler(std::shared_ptr<FontHandler> handler) { fontHandler = handler; }
    
    const auto& getString() const { return utf8String; }
    void setString(std::string str) { utf8String = str; needsUpdateGeometry = true; }
    
    auto getFontSize() const  { return fontSize; }
    void setFontSize(size_t size) { fontSize = size; needsUpdateGeometry = true; }
    
    auto getDefaultColor() const  { return defaultColor; }
    void setDefaultColor(sf::Color c) { defaultColor = c; needsUpdateGeometry = true; }
    
    auto getDefaultOutlineColor() const  { return defaultOutlineColor; }
    void setDefaultOutlineColor(sf::Color c) { defaultOutlineColor = c; needsUpdateGeometry = true; }
    
    auto getOutlineThickness() const  { return outlineThickness; }
    void setOutlineThickness(float thickness) { outlineThickness = thickness; needsUpdateGeometry = true; }
    
    auto getWordWrappingWidth() const { return wordWrappingWidth; }
    void setWordWrappingWidth(float width) { wordWrappingWidth = width; needsUpdateGeometry = true; }
    
    auto getWordWrappingAlgorithm() const { return wordWrappingAlgorithm; }
    void setWordWrappingAlgorithm(WordWrappingAlgorithm algorithm)
    { 
        wordWrappingAlgorithm = algorithm;
        needsUpdateGeometry = true;
    }
    
    float getLineSpacing() const;
    float getHeightForLineNumber(size_t lines) const;
    
    auto getWordAlignment() const { return wordAlignment; }
    void setWordAlignment(Alignment alignment) { wordAlignment = alignment; needsUpdateGeometry = true; }
    
    auto getHorizontalAnchor() const { return horizontalAnchor; }
    void setHorizontalAnchor(HorAnchor anchor) { horizontalAnchor = anchor; needsUpdateGeometry = true; }
    
    auto getVerticalAnchor() const { return verticalAnchor; }
    void setVerticalAnchor(VertAnchor anchor) { verticalAnchor = anchor; needsUpdateGeometry = true; }
    
    auto getRTL() const { return rtl; }
    void setRTL(bool isRTL) { rtl = isRTL; needsUpdateGeometry = true; }

    const auto& getLocalBounds() const { return bounds; }
    
    void buildGeometry();

    GraphemeRange getGraphemeClusterInterval(size_t begin, size_t end, bool outline = false);
    GraphemeRange getGraphemeCluster(size_t index, bool outline = false);
    size_t getGraphemeClusterByteLocation(size_t index) const;
    size_t getGraphemeClusterLine(size_t index) const;
    size_t getNumberOfGraphemeClusters() const;

    size_t getLineBoundary(size_t index) const;
    size_t getNumberOfLines() const;
    
    GraphemeRange getAllVertices(bool outline = false);
    
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
