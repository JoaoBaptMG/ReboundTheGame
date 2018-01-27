#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <utility>
#include <string>
#include <map>
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
    size_t prevStringLength = 0;
    
public:
    TextDrawable(std::shared_ptr<FontHandler> fontHandler = nullptr);
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
    
    GraphemeRange getGraphemeCluster(size_t index, bool outline = false);
    size_t getNumberOfGraphemeClusters() const;
    
    GraphemeRange getAllVertices(bool outline = false);
    
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
