#pragma once

#include <SFML/Graphics.hpp>
#include <chronoUtils.hpp>
#include <memory>

using OpaqueWaterPtr = std::unique_ptr<sf::RenderTexture, void(*)(const sf::RenderTexture*)>;

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
    FrameTime startingTime, curTime;
    sf::Vertex quad[4];
    sf::Color color, coastColor;

    OpaqueWaterPtr previousFrame2, previousFrame, curFrame, newVelocity;

    intmax_t curT;
    mutable bool haltSimulation;
    bool topHidden;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
    WaterBody(sf::Vector2f drawingSize);
    ~WaterBody() {}

    void recreateQuad();
    void resetWaves();
    void resetSimulationTextures();
    void update(FrameTime curTime);
    void updateSimulation();

    void setVelocity(float point, float newVel);

    auto getDrawingSize() const { return drawingSize; }
    void setDrawingSize(sf::Vector2f size) { drawingSize = size; recreateQuad(); resetSimulationTextures(); }

    auto getColor() const { return color; }
    void setColor(sf::Color color) { this->color = color; }

    auto getCoastColor() const { return coastColor; }
    void setCoastColor(sf::Color color) { coastColor = color; }
    
    auto getTopHidden() const { return topHidden; }
    void setTopHidden(bool top) { topHidden = top; recreateQuad(); resetWaves(); resetSimulationTextures(); }
};
