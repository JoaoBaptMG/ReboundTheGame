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


#include <chronoUtils.hpp>

#include <generic_ptrs.hpp>
#include <streamReaders.hpp>
#include "particles/ParticleBatch.hpp"

class ParticleEmitter final
{
    ParticleBatch::Style particleStyle;
    ParticleBatch::Duration totalLifetime;
    ParticleBatch::Duration emissionPeriod;

    glm::vec2 emissionCenter;
    glm::vec2 emissionHalfSize;
    glm::vec2 acceleration;
    float emissionInnerLimit;

    float directionFirst, directionSecond, directionCenterWeight;
    float speedFirst, speedSecond, speedWeight;
    float sizeBeginFirst, sizeBeginSecond, sizeBeginWeight;
    float sizeEndFirst, sizeEndSecond, sizeEndWeight;
    
    ParticleBatch::Duration lifetimeFirst, lifetimeSecond;

    glm::u8vec4 colorBeginFirst, colorBeginSecond;
    glm::u8vec4 colorEndFirst, colorEndSecond;
    bool generateHSV;

public:
	ParticleEmitter() {}

    void generateNewParticles(ParticleBatch& batch, ParticleBatch::Duration cur, ParticleBatch::Duration last) const;
    auto getTotalLifetime() const { return totalLifetime; }
    auto getParticleStyle() const { return particleStyle; }

    friend bool readFromStream(InputStream& stream, ParticleEmitter& in);
};

using ParticleEmitterSet = std::unordered_map<std::string,ParticleEmitter>;

util::generic_shared_ptr loadParticleEmitterList(std::unique_ptr<InputStream>& stream);
