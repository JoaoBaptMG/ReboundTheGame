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
#include <vector>
#include <chronoUtils.hpp>
#include <memory>
#include <unordered_map>

#include "objects/GameObject.hpp"

class GameScene;
class Renderer;

class ParticleEmitter;
using ParticleEmitterSet = std::unordered_map<std::string,ParticleEmitter>;

class ParticleBatch final : public GameObject
{
public:
    enum class Style : uint8_t { Smooth, Disk, MaxSize };

private:
    static sf::Shader& getParticleShader(Style style);
    
public:
    using TimePoint = std::chrono::steady_clock::time_point;
    using Duration = TimePoint::duration;

    struct PositionInfo
    {
        sf::Vector2f position, velocity, acceleration;
    };

    struct DisplayInfo
    {
        sf::Glsl::Vec4 beginColor, endColor, curColor;
        float beginSize, endSize, curSize;
    };

    struct TimeInfo
    {
        TimePoint beginTime, endTime;
        Duration lifetime;
    };

private:
	std::function<float()> generator;

    sf::VertexArray vertices;

    std::vector<PositionInfo> positionAttributes;
    std::vector<DisplayInfo> displayAttributes;
    std::vector<TimeInfo> lifeAttributes;

    std::shared_ptr<ParticleEmitterSet> emitterSet;
    ParticleEmitter* emitter;
    
    FrameTime lastTime, initialTime;
    size_t drawingDepth;

    sf::Vector2f position;
    bool aborted;
    
public:
    ParticleBatch(GameScene& scene, std::string emitterSetName, std::string emitterName,
        bool persistent = false, size_t depth = 12);
    ParticleBatch(GameScene& scene, std::string emitterSetName, std::string emitterName, size_t depth,
        bool persistent = false) : ParticleBatch(scene, emitterSetName, emitterName, persistent, depth) {}
    virtual ~ParticleBatch();

    void addParticle(PositionInfo pos, DisplayInfo display, Duration lifetime);
    void removeParticle(size_t index);

    template <class Rep, class Period>
    inline void addParticle(PositionInfo pos, DisplayInfo display, const std::chrono::duration<Rep,Period>& lt)
    {
        addParticle(pos, display, std::chrono::duration_cast<Duration>(lt));
    }

    virtual void update(FrameTime curTime) override;
    virtual void render(Renderer& renderer) override;
    virtual bool notifyScreenTransition(cpVect displacement) override;

    void abort() { aborted = true; }
    void unabort() { aborted = false; }

    auto getPosition() { return position; }
    void setPosition(sf::Vector2f pos) { position = pos; }

	friend class ParticleEmitter;
};
