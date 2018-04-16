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


#include "WaterBody.hpp"

#include <random>
#include <functional>
#include <SFML/OpenGL.hpp>

#include <assert.hpp>
#include <chronoUtils.hpp>

#include <vector>
#include <utility>
#include <predUtils.hpp>

#include <iostream>
#include <cassert>

constexpr float MaxSineWaveHeight = 4;
constexpr float MaxSineWaveK = 0.08f;
constexpr float MaxSineWaveOmega = 0.08f;

constexpr float WaveSpeed = 0.92;

constexpr auto ShaderUtilities = R"util(
float decomposeColor(vec4 color)
{
    float cl = color.r * 255.0;
    float ch = color.g * 255.0;
    float sgn = color.b;

    return (step(sgn, 0.5) * 2.0 - 1.0) * (ch + cl / 256.0);
}

vec4 composeColor(float val)
{
    float ch = floor(abs(val));
    float cl = floor((abs(val)-ch)*256.0);
    float sgn = step(val, 0.0);

    return vec4(cl/255.0, ch/255.0, sgn, 1.0);
}
)util";

constexpr auto WaveDrawingShader = R"wavedraw(
uniform sampler2D dynamicTex;
uniform vec4 color, coastColor;
uniform vec4 staticWaveProperties[8];
uniform float t, texWidth;

void main()
{
    float x = gl_TexCoord[0].x;
    float staticDist = 0.0;
    for (int i = 0; i < 8; i++)
    {
        staticDist += staticWaveProperties[i].x + staticWaveProperties[i].y *
                      sin(staticWaveProperties[i].z * x + staticWaveProperties[i].w * t);
    }

    float dist = staticDist + decomposeColor(texture2D(dynamicTex, vec2(gl_TexCoord[0].x/texWidth, 0.0)));
    float weight = step(dist, gl_TexCoord[0].y);
    float mixed = step(dist + 4.0, gl_TexCoord[0].y);

    gl_FragColor = mix(coastColor, color, mixed) * weight;
}
)wavedraw";

constexpr auto WaveUpdateShader = R"waveupd(
uniform sampler2D prevFrame2, prevFrame, newVelocity;
uniform float c, texWidth;

void main()
{
    vec2 tex = gl_TexCoord[0].xy;
    vec2 texn = tex - vec2(1.0, 0.0);
    vec2 texp = tex + vec2(1.0, 0.0);

    float fxt = decomposeColor(texture2D(prevFrame, tex/texWidth));
    float fxnt = decomposeColor(texture2D(prevFrame, texn/texWidth));
    float fxpt = decomposeColor(texture2D(prevFrame, texp/texWidth));
    float fxtn = decomposeColor(texture2D(prevFrame2, tex/texWidth));
    float vel = decomposeColor(texture2D(newVelocity, tex/texWidth));

    float new = 0.99*(2.0*fxt - fxtn) + c*c*(fxnt + fxpt - 2.0*fxt) - vel;
    gl_FragColor = composeColor(new);
}
)waveupd";

sf::Shader& WaterBody::getSimulationShader()
{
    static sf::Shader shader;
    static bool shaderLoaded = false;

    if (!shaderLoaded)
    {
        auto shaderStr = std::string(ShaderUtilities) + std::string(WaveUpdateShader);
        ASSERT(shader.loadFromMemory(shaderStr, sf::Shader::Fragment));
        shaderLoaded = true;
    }

    return shader;
}

sf::Shader& WaterBody::getDrawingShader()
{
    static sf::Shader shader;
    static bool shaderLoaded = false;

    if (!shaderLoaded)
    {
        auto shaderStr = std::string(ShaderUtilities) + std::string(WaveDrawingShader);
        ASSERT(shader.loadFromMemory(shaderStr, sf::Shader::Fragment));
        shaderLoaded = true;
    }

    return shader;
}

static OpaqueWaterPtr getSpareWaterTexture()
{
    static std::vector<std::pair<std::unique_ptr<sf::RenderTexture>,bool>> cachedTextures{};

    auto returnTexture = [](const sf::RenderTexture* texture)
    {
        auto it = std::find_if(cachedTextures.begin(), cachedTextures.end(), PRED_X(x.first.get() == texture));
        it->second = false;
        std::cout << "Returning texture " << texture << "." << std::endl;
    };

    auto freeSlot = std::find_if(cachedTextures.begin(), cachedTextures.end(), PRED_X(!x.second));
    if (freeSlot == cachedTextures.end())
    {
        for (size_t i = 0; i < 16; i++)
        {
            auto ptr = std::make_unique<sf::RenderTexture>();
            ASSERT(ptr->create(1024, 1));
            cachedTextures.emplace_back(std::move(ptr), false);
        }

        freeSlot = std::find_if(cachedTextures.begin(), cachedTextures.end(), PRED_X(!x.second));
    }

    freeSlot->second = true;
    std::cout << "Getting texture " << freeSlot->first.get() << "." << std::endl;
    return OpaqueWaterPtr(freeSlot->first.get(), returnTexture);
}

WaterBody::WaterBody(sf::Vector2f drawingSize) : drawingSize(drawingSize), curT(0), haltSimulation(false),
                                                 previousFrame2(nullptr, [](const sf::RenderTexture*){}),
                                                 previousFrame(nullptr, [](const sf::RenderTexture*){}),
                                                 curFrame(nullptr, [](const sf::RenderTexture*){}),
                                                 newVelocity(nullptr, [](const sf::RenderTexture*){})
{
    recreateQuad();
    resetWaves();
    resetSimulationTextures();
}

void WaterBody::recreateQuad()
{
    quad[0].position = quad[0].texCoords = sf::Vector2f(0, -256);
    quad[1].position = quad[1].texCoords = sf::Vector2f(0, drawingSize.y);
    quad[2].position = quad[2].texCoords = sf::Vector2f(drawingSize.x, drawingSize.y);
    quad[3].position = quad[3].texCoords = sf::Vector2f(drawingSize.x, -256);
    
    if (topHidden)
        quad[0].position.y = quad[3].position.y = 0;

    for (auto& vtx : quad)
        vtx.color = sf::Color::White;
}

void WaterBody::resetWaves()
{
    if (!topHidden)
    {
        std::random_device dev;

        std::mt19937_64 gen(dev());
        std::uniform_real_distribution<float> dist;
        auto random = std::bind(dist, gen);

        for (auto& wave : generatedWaves)
        {
            wave.ofs = MaxSineWaveHeight * (2.0f * random() - 1.0f);
            wave.amp = MaxSineWaveHeight * random();
            wave.k = MaxSineWaveK * random();
            wave.omega = MaxSineWaveOmega * (2.0f * random() - 1.0f);
        }
    }
    else
    {
        for (auto& wave : generatedWaves)
        {
            wave.ofs = -256;
            wave.amp = wave.k = wave.omega = 0;
        }
    }
}

void WaterBody::resetSimulationTextures()
{
    if (!topHidden)
    {
        for (auto texture : {&curFrame, &previousFrame, &previousFrame2, &newVelocity})
            if (*texture == nullptr)
            {
                *texture = getSpareWaterTexture();
                (*texture)->clear(sf::Color::Black);
                (*texture)->display();
            }
    }
    else for (auto texture : { &curFrame, &previousFrame, &previousFrame2, &newVelocity })
            texture->reset();
}

void WaterBody::update(FrameTime curTime)
{
    if (startingTime == decltype(startingTime)())
    {
        curT = 0;
        startingTime = curTime;
    }

    curT = (curTime - startingTime)/UpdatePeriod;

    if (!topHidden) updateSimulation();
}

void WaterBody::updateSimulation()
{
    if (haltSimulation) return;

    newVelocity->display();

    sf::Vertex line[2];
    line[0].position = line[0].texCoords = sf::Vector2f(0, 0);
    line[1].position = line[1].texCoords = sf::Vector2f(drawingSize.x, 0);

    {
        using std::swap;
        previousFrame2.swap(previousFrame);
        previousFrame.swap(curFrame);
    }

    auto& shader = getSimulationShader();
    shader.setUniform("newVelocity", newVelocity->getTexture());
    shader.setUniform("prevFrame2", previousFrame2->getTexture());
    shader.setUniform("prevFrame", previousFrame->getTexture());
    shader.setUniform("c", WaveSpeed);
    shader.setUniform("texWidth", (float)curFrame->getSize().x);

    sf::RenderStates states;
    states.shader = &shader;
    curFrame->draw(line, 2, sf::Lines, states);
    curFrame->display();

    newVelocity->clear(sf::Color::Black);

    haltSimulation = true;
}

void WaterBody::setVelocity(float point, float newVel)
{
    if (topHidden) return;

    if (newVel > 255) newVel = 255;
    else if (newVel < -255) newVel = -255;

    sf::Color color;
    color.r = floorf(fmodf(fabsf(newVel)*256, 256));
    color.g = floorf(fabsf(newVel));
    color.b = 255 * (newVel >= 0);
    color.a = 255;

    sf::Vertex pt(sf::Vector2f(point, 0), color);

    newVelocity->draw(&pt, 1, sf::Points);
}

void WaterBody::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    haltSimulation = false;

    auto& shader = getDrawingShader();
    shader.setUniform("color", sf::Glsl::Vec4(color));
    shader.setUniform("coastColor", sf::Glsl::Vec4(coastColor));
    shader.setUniformArray("staticWaveProperties", reinterpret_cast<const sf::Glsl::Vec4*>(generatedWaves), 8);
    shader.setUniform("t", (float)curT);
    if (!topHidden)
    {
        shader.setUniform("texWidth", (float)curFrame->getSize().x);
        shader.setUniform("dynamicTex", curFrame->getTexture());
    }

    states.shader = &shader;
    target.draw(quad, 4, sf::TriangleFan, states);
}
