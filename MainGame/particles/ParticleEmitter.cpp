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


#include "ParticleEmitter.hpp"

#include <chronoUtils.hpp>
#include <vector_math.hpp>

#include <utility>
#include <limits>

using namespace std::literals::chrono_literals;

bool readFromStream(InputStream& stream, ParticleEmitter& in)
{
    float lifetimeSeconds, emissionPeriodSeconds;
    float firstSeconds, secondSeconds;
    uint8_t hsv;

    if (!readFromStream(stream, in.particleStyle, lifetimeSeconds, emissionPeriodSeconds, in.emissionCenter,
                        in.emissionHalfSize, in.acceleration, in.emissionInnerLimit,
                        in.directionFirst, in.directionSecond, in.directionCenterWeight,
                        in.speedFirst, in.speedSecond, in.speedWeight,
                        in.sizeBeginFirst, in.sizeBeginSecond, in.sizeBeginWeight,
                        in.sizeEndFirst, in.sizeEndSecond, in.sizeEndWeight,
                        firstSeconds, secondSeconds,
                        in.colorBeginFirst, in.colorBeginSecond,
                        in.colorEndFirst, in.colorEndSecond, hsv)) return false;

    if (lifetimeSeconds == std::numeric_limits<float>::infinity()) in.totalLifetime = ParticleBatch::Duration::max();
    else in.totalLifetime = std::chrono::duration_cast<ParticleBatch::Duration>(FloatSeconds(lifetimeSeconds));
    
    in.emissionPeriod = std::chrono::duration_cast<ParticleBatch::Duration>(FloatSeconds(emissionPeriodSeconds));
    in.lifetimeFirst = std::chrono::duration_cast<ParticleBatch::Duration>(FloatSeconds(firstSeconds));
    in.lifetimeSecond = std::chrono::duration_cast<ParticleBatch::Duration>(FloatSeconds(secondSeconds));
    in.generateHSV = hsv;

    return true;
}

util::generic_shared_ptr loadParticleEmitterList(std::unique_ptr<InputStream>& stream)
{
    std::shared_ptr<ParticleEmitterSet> content{new ParticleEmitterSet};

    if (checkMagic(*stream, "PEMIT") && readFromStream(*stream, *content))
        return util::generic_shared_ptr{content};

    return util::generic_shared_ptr{};
}

std::tuple<float,float,float> RGBtoHSV(sf::Color c)
{
    float r = c.r/255.0f, g = c.g/255.0f, b = c.b/255.0f;
    
    auto max = std::max(r, std::max(g, b));
    auto min = std::min(r, std::min(g, b));

    float h;
    if (max == r) h = float(g - b) / (max - min);
    else if (max == g) h = float(b - r) / (max - min) + 2;
    else if (max == b) h = float(r - g) / (max - min) + 4;
    if (h < 0) h += 6;
    h *= 60;

    float s = float(max - min) / max;
    float v = float(max);

    return std::make_tuple(h, s, v);
}

sf::Color HSVtoRGB(float h, float s, float v, char a)
{
    float f = h/60 - floor(h/60);

    float p = v * (1 - s);
    float q = v * (1 - f*s);
    float t = v * (1 - (1-f)*s);

    switch (size_t(h/60))
    {
        case 0: return sf::Color(255.0 * v, 255.0 * t, 255.0 * p, a);
        case 1: return sf::Color(255.0 * q, 255.0 * v, 255.0 * p, a);
        case 2: return sf::Color(255.0 * p, 255.0 * v, 255.0 * t, a);
        case 3: return sf::Color(255.0 * p, 255.0 * q, 255.0 * v, a);
        case 4: return sf::Color(255.0 * t, 255.0 * p, 255.0 * v, a);
        case 5: return sf::Color(255.0 * v, 255.0 * p, 255.0 * q, a);
        default: return sf::Color::Black;
    }
}

sf::Color randomColor(std::function<float()> &generator, sf::Color c1, sf::Color c2, bool hsv)
{
    if (!hsv)
    {
        return sf::Color(c1.r + generator() * (c2.r - c1.r),
                         c1.g + generator() * (c2.g - c1.g),
                         c1.b + generator() * (c2.b - c1.b),
                         c1.a + generator() * (c2.a - c1.a));
    }
    else
    {
        float h1, s1, v1, h2, s2, v2;
        std::tie(h1, s1, v1) = RGBtoHSV(c1);
        std::tie(h2, s2, v2) = RGBtoHSV(c2);

        float h = h1 + generator() * (h2 - h1);
        float s = s1 + generator() * (s2 - s1);
        float v = v1 + generator() * (v2 - v1);

        return HSVtoRGB(h, s, v, c1.a + generator() * (c2.a - c1.a));
    }
}

float signedPowf(float x, float y)
{
    float up = powf(fabsf(x), y);
    return x < 0 ? -up : up;
}

void ParticleEmitter::generateNewParticles(ParticleBatch& batch,
    ParticleBatch::Duration cur, ParticleBatch::Duration last) const
{
	auto& generator = batch.generator;

    size_t numParticles = cur/emissionPeriod - last/emissionPeriod;

    for (size_t i = 0; i < numParticles; i++)
    {
        ParticleBatch::PositionInfo posInfo;

        float insq = emissionInnerLimit * emissionInnerLimit;
        float radius = sqrtf(insq + generator()*(1 - insq));
        float angle = generator() * 2 * M_PI;
        posInfo.position = fromRadiusAngleRadians(radius, angle);
        posInfo.position.x *= emissionHalfSize.x;
        posInfo.position.y *= emissionHalfSize.y;
        posInfo.position += emissionCenter;

        float middle = (directionFirst + directionSecond) / 2;
        float direction = middle +
            signedPowf(generator() * 2.0 - 1.0, directionCenterWeight) * (directionSecond - middle);
        float speed = speedFirst + powf(generator(), speedWeight) * (speedSecond - speedFirst);
        posInfo.velocity = fromRadiusAngleDegrees(speed, direction);

        posInfo.acceleration = acceleration;

        ParticleBatch::DisplayInfo displayInfo;
        displayInfo.beginColor = randomColor(generator, colorBeginFirst, colorBeginSecond, generateHSV);
        displayInfo.endColor = randomColor(generator, colorEndFirst, colorEndSecond, generateHSV);
        displayInfo.beginSize = sizeBeginFirst + powf(generator(), sizeBeginWeight) * (sizeBeginSecond - sizeBeginFirst);
        displayInfo.endSize = sizeEndFirst + powf(generator(), sizeEndWeight) * (sizeEndSecond - sizeEndFirst);

        auto lifetime = lifetimeFirst + generator() * (lifetimeSecond - lifetimeFirst);
        batch.addParticle(posInfo, displayInfo, lifetime);
    }
}
