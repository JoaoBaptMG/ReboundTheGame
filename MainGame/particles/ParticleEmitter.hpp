#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>

#include <generic_ptrs.hpp>
#include <streamReaders.hpp>
#include "particles/ParticleBatch.hpp"

class ParticleEmitter final
{
    ParticleBatch::Duration totalLifetime;
    ParticleBatch::Duration emissionPeriod;

    sf::Vector2f emissionCenter;
    sf::Vector2f emissionHalfSize;
    sf::Vector2f acceleration;
    float emissionInnerLimit;

    float directionFirst, directionSecond, directionCenterWeight;
    float speedFirst, speedSecond, speedWeight;
    float sizeBeginFirst, sizeBeginSecond, sizeBeginWeight;
    float sizeEndFirst, sizeEndSecond, sizeEndWeight;
    
    ParticleBatch::Duration lifetimeFirst, lifetimeSecond;

    sf::Color colorBeginFirst, colorBeginSecond;
    sf::Color colorEndFirst, colorEndSecond;
    bool generateHSV;

public:
	ParticleEmitter() {}

    void generateNewParticles(ParticleBatch& batch, ParticleBatch::Duration cur, ParticleBatch::Duration last) const;
    auto getTotalLifetime() const { return totalLifetime; }

    friend bool readFromStream(sf::InputStream& stream, ParticleEmitter& in);
};

using ParticleEmitterSet = std::unordered_map<std::string,ParticleEmitter>;

util::generic_shared_ptr loadParticleEmitterList(std::unique_ptr<sf::InputStream>& stream);
