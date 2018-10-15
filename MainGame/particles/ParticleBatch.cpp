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


#include "ParticleBatch.hpp"

#include <random>
#include <functional>

#include "particles/ParticleEmitter.hpp"
#include "rendering/Renderer.hpp"
#include <chronoUtils.hpp>
#include "scene/GameScene.hpp"
#include "resources/ResourceManager.hpp"
#include <assert.hpp>

constexpr auto ParticleVertexShader = R"vertex(
varying float PointSize;
varying vec2 TexCoord;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    PointSize = gl_MultiTexCoord0.y;
    
    float texId = gl_MultiTexCoord0.x;
    TexCoord = vec2(mod(texId,2.0), floor(texId/2.0));
    
    gl_FrontColor = gl_Color;
}
)vertex";

constexpr auto ParticleSmoothFragmentShader = R"fragment(
varying vec2 TexCoord;

void main()
{
    gl_FragColor = gl_Color;
    gl_FragColor.a *= 1.0 - 2.0 * distance(TexCoord, vec2(0.5, 0.5));
}
)fragment";

constexpr auto ParticleDiskFragmentShader = R"fragment(
varying float PointSize;
varying vec2 TexCoord;

void main()
{
    gl_FragColor = gl_Color;
    gl_FragColor.a *= clamp((1.0 - 2.0 * distance(TexCoord, vec2(0.5, 0.5))) * PointSize, 0.0, 1.0);
}
)fragment";

const char* ParticleFragmentShaders[] = { ParticleSmoothFragmentShader, ParticleDiskFragmentShader };

sf::Shader& ParticleBatch::getParticleShader(ParticleBatch::Style style)
{
    static sf::Shader shaders[(size_t)Style::MaxSize];
    static bool shadersLoaded[(size_t)Style::MaxSize];

    if (!shadersLoaded[(size_t)style])
    {
        ASSERT(shaders[(size_t)style].loadFromMemory(ParticleVertexShader, ParticleFragmentShaders[(size_t)style]));
        shadersLoaded[(size_t)style] = true;
    }

    return shaders[(size_t)style];
}

static sf::Glsl::Vec4 operator+(sf::Glsl::Vec4 v1, sf::Glsl::Vec4 v2)
{
    return sf::Glsl::Vec4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}

static sf::Glsl::Vec4 operator-(sf::Glsl::Vec4 v1, sf::Glsl::Vec4 v2)
{
    return sf::Glsl::Vec4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
}

static sf::Glsl::Vec4 operator*(float s, sf::Glsl::Vec4 v2)
{
    return sf::Glsl::Vec4(s * v2.x, s * v2.y, s * v2.z, s * v2.w);
}

inline static auto convertDuration(FrameDuration duration)
{
    return std::chrono::duration_cast<ParticleBatch::Duration>(duration);
}

inline static auto convertTime(FrameTime time)
{
    using namespace std::chrono;
    return ParticleBatch::TimePoint(convertDuration(time.time_since_epoch()));
}

ParticleBatch::ParticleBatch(GameScene &scene, std::string emitterSetName, std::string emitterName,
    bool persistent, size_t depth)
    : GameObject(scene), vertices(sf::Triangles), drawingDepth(depth), aborted(false),
      emitterSet(scene.getResourceManager().load<ParticleEmitterSet>(emitterSetName))
{
	std::random_device init;
	std::mt19937 rgen(init());
	std::uniform_real_distribution<float> distribution;
	generator = std::bind(distribution, rgen);

    isPersistent = persistent;
    emitter = &emitterSet->at(emitterName);
}

ParticleBatch::~ParticleBatch()
{
}

void ParticleBatch::addParticle(ParticleBatch::PositionInfo pos,
                                ParticleBatch::DisplayInfo display,
                                ParticleBatch::Duration lifetime)
{
    pos.position += position;
    positionAttributes.push_back(pos);
    displayAttributes.push_back(display);
    lifeAttributes.push_back({ convertTime(lastTime), convertTime(lastTime) + lifetime, lifetime });
    vertices.resize(6*positionAttributes.size());
}

void ParticleBatch::removeParticle(size_t index)
{
    using std::swap;

    auto last = positionAttributes.size()-1;
    swap(positionAttributes[index], positionAttributes[last]);
    swap(displayAttributes[index], displayAttributes[last]);
    swap(lifeAttributes[index], lifeAttributes[last]);

    positionAttributes.pop_back();
    displayAttributes.pop_back();
    lifeAttributes.pop_back();
    vertices.resize(6*positionAttributes.size());
}

void ParticleBatch::update(FrameTime curTime)
{
    if (lastTime == decltype(lastTime)()) lastTime = curTime;
    if (initialTime == decltype(initialTime)()) initialTime = curTime;

    auto dt = toSeconds<float>(curTime - lastTime);
    
    for (auto& data : positionAttributes)
    {
        data.position += data.velocity * dt;
        data.velocity += data.acceleration * dt;
    }

    for (size_t i = 0; i < displayAttributes.size(); i++)
    {
        auto& display = displayAttributes[i];
        auto& life = lifeAttributes[i];
        
        auto factor = toSeconds<float>(convertTime(curTime) - life.beginTime) / toSeconds<float>(life.lifetime);
        if (factor >= 1.0) factor = 1.0;
        display.curColor = display.beginColor + factor * (display.endColor - display.beginColor);
        display.curSize = display.beginSize + factor * (display.endSize - display.beginSize);
    }

    for (size_t i = 0; i < lifeAttributes.size(); i++)
        if (convertTime(curTime) > lifeAttributes[i].endTime) removeParticle(i);

    if (!aborted && curTime - initialTime <= emitter->getTotalLifetime())
        emitter->generateNewParticles(*this, convertDuration(curTime - initialTime), convertDuration(lastTime - initialTime));
    else if (positionAttributes.empty()) remove();

    lastTime = curTime;
}

bool ParticleBatch::notifyScreenTransition(cpVect displacement)
{
    for (auto& data : positionAttributes)
        data.position += sf::Vector2f(displacement.x, displacement.y);
    
    return true;
}

void ParticleBatch::render(Renderer& renderer)
{
    for (size_t i = 0; i < positionAttributes.size(); i++)
    {
        for (size_t k = 0; k < 6; k++)
        {
            vertices[6*i+k].position = positionAttributes[i].position;
            vertices[6*i+k].color = sf::Color(displayAttributes[i].curColor.x * 255.f,
                                              displayAttributes[i].curColor.y * 255.f,
                                              displayAttributes[i].curColor.z * 255.f,
                                              displayAttributes[i].curColor.w * 255.f);
        }
        
        auto curSize = displayAttributes[i].curSize;
        
        vertices[6*i+0].position += sf::Vector2f(-curSize/2, -curSize/2);
        vertices[6*i+1].position += sf::Vector2f(+curSize/2, -curSize/2);
        vertices[6*i+2].position += sf::Vector2f(+curSize/2, +curSize/2);
        vertices[6*i+3].position += sf::Vector2f(-curSize/2, +curSize/2);
        vertices[6*i+4].position += sf::Vector2f(-curSize/2, -curSize/2);
        vertices[6*i+5].position += sf::Vector2f(+curSize/2, +curSize/2);
        
        vertices[6*i+0].texCoords = sf::Vector2f(0, curSize);
        vertices[6*i+1].texCoords = sf::Vector2f(1, curSize);
        vertices[6*i+2].texCoords = sf::Vector2f(3, curSize);
        vertices[6*i+3].texCoords = sf::Vector2f(2, curSize);
        vertices[6*i+4].texCoords = sf::Vector2f(0, curSize);
        vertices[6*i+5].texCoords = sf::Vector2f(3, curSize);
    }

    sf::RenderStates states;
    states.blendMode = sf::BlendAlpha;
    states.shader = &getParticleShader(emitter->getParticleStyle());
    renderer.pushDrawable(vertices, states, drawingDepth);
}
