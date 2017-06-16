#include "ParticleBatch.hpp"

#include <random>
#include <functional>

#include "particles/ParticleEmitter.hpp"
#include "rendering/Renderer.hpp"
#include "utility/chronoUtils.hpp"
#include "scene/GameScene.hpp"
#include "resources/ResourceManager.hpp"
#include "utility/assert.hpp"

constexpr auto ParticleVertexShader = R"vertex(
#version 130
void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_PointSize = gl_MultiTexCoord0.x;
    gl_FrontColor = gl_Color;
}
)vertex";

constexpr auto ParticleFragmentShader = R"fragment(
#version 130
void main()
{
    gl_FragColor = gl_Color;
    gl_FragColor.a *= 1.0 - 2.0 * distance(gl_PointCoord, vec2(0.5, 0.5));
}
)fragment";

sf::Shader& ParticleBatch::getParticleShader()
{
    static sf::Shader shader;
    static bool shaderLoaded = false;

    if (!shaderLoaded)
    {
        ASSERT(shader.loadFromMemory(ParticleVertexShader, ParticleFragmentShader));
        shaderLoaded = true;
    }

    return shader;
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

ParticleBatch::ParticleBatch(GameScene &scene, std::string emitterSetName, std::string emitterName,
    bool persistent, size_t depth)
    : GameObject(scene), vertices(sf::Points), drawingDepth(depth), aborted(false),
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
    lifeAttributes.push_back({ lastTime, lastTime + lifetime, lifetime });
    vertices.resize(vertices.getVertexCount()+1);
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
    vertices.resize(last);
}

void ParticleBatch::update(std::chrono::steady_clock::time_point curTime)
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
        
        auto factor = toSeconds<float>(curTime - life.beginTime) / toSeconds<float>(life.lifetime);
        if (factor >= 1.0) factor = 1.0;
        display.curColor = display.beginColor + factor * (display.endColor - display.beginColor);
        display.curSize = display.beginSize + factor * (display.endSize - display.beginSize);
    }

    for (size_t i = 0; i < lifeAttributes.size(); i++)
        if (curTime > lifeAttributes[i].endTime) removeParticle(i);

    if (!aborted && curTime - initialTime <= emitter->getTotalLifetime())
        emitter->generateNewParticles(*this, curTime - initialTime, lastTime - initialTime);
    else if (positionAttributes.empty()) remove();

    lastTime = curTime;
}

void ParticleBatch::render(Renderer& renderer)
{
    for (size_t i = 0; i < vertices.getVertexCount(); i++)
    {
        vertices[i].position = positionAttributes[i].position;
        vertices[i].texCoords = sf::Vector2f(displayAttributes[i].curSize, 0);
        vertices[i].color = sf::Color(displayAttributes[i].curColor.x * 255.f,
                                      displayAttributes[i].curColor.y * 255.f,
                                      displayAttributes[i].curColor.z * 255.f,
                                      displayAttributes[i].curColor.w * 255.f);
    }

    sf::RenderStates states;
    states.blendMode = sf::BlendAlpha;
    states.shader = &getParticleShader();
    renderer.pushDrawable(vertices, states, drawingDepth);
}
