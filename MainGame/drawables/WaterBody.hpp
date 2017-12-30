#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>

class WaterBody final : public sf::Drawable
{
    static sf::Shader& getSimulationShader();
    static sf::Shader& getDrawingShader();

#pragma pack(push, 1)
    struct StaticWaveProperties
    {
        float ofs, amp, k, omega;
        operator sf::Glsl::Vec4()
        {
            return sf::Glsl::Vec4{ofs, amp, k, omega};
        }
    };
#pragma pack(pop)

    StaticWaveProperties generatedWaves[8];

    sf::Vector2f drawingSize;
    std::chrono::steady_clock::time_point startingTime;
    sf::Vertex quad[4];
    sf::Color color, coastColor;

    intmax_t curT;
    bool topHidden;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
    WaterBody(sf::Vector2f drawingSize);
    ~WaterBody() {}

    void recreateQuad();
    void resetWaves();
    void update(std::chrono::steady_clock::time_point curTime);

    auto getDrawingSize() const { return drawingSize; }
    void setDrawingSize(sf::Vector2f size) { drawingSize = size; recreateQuad(); }

    auto getColor() const { return color; }
    void setColor(sf::Color color) { this->color = color; }

    auto getCoastColor() const { return coastColor; }
    void setCoastColor(sf::Color color) { coastColor = color; }
    
    auto getTopHidden() const { return topHidden; }
    void setTopHidden(bool top) { topHidden = top; recreateQuad(); resetWaves(); }
};
