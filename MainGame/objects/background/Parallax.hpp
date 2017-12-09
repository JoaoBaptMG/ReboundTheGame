#pragma once

#include "objects/GameObject.hpp"
#include "drawables/WrapPlane.hpp"
#include "resources/ResourceManager.hpp"

#include <SFML/System.hpp>
#include <string>

class GameScene;
class Renderer;

namespace background
{
    class Parallax final : public ::GameObject
    {
        WrapPlane plane;
        sf::Vector2f internalDisplacement;
        float parallaxFactor;

    public:
        Parallax(GameScene& gameScene);
        Parallax(GameScene& gameScene, std::string textureName);
        virtual ~Parallax() {}

        virtual void update(std::chrono::steady_clock::time_point curTime);
        virtual void render(Renderer& renderer);
        virtual bool notifyScreenTransition(cpVect displacement);

        auto getParallaxFactor() { return parallaxFactor; }
        void setParallaxFactor(float factor) { parallaxFactor = factor; }

        struct ConfigStruct
        {
            sf::Vector2<int16_t> dummy;
            std::string textureName;
            int32_t parallaxNumerator, parallaxDenominator;
        };

        bool configure(const ConfigStruct& config);
    };
}

bool readFromStream(sf::InputStream& stream, background::Parallax::ConfigStruct& config);
