#pragma once

#include "objects/GameObject.hpp"
#include "drawables/Sprite.hpp"

#include <SFML/Graphics.hpp>
#include <chrono>

class GameScene;
class Renderer;

namespace enemies
{
    class Floater final : public ::GameObject
    {
        sf::Vector2f position;
        float yDisplacement;
        Sprite sprite;

        std::chrono::steady_clock::time_point initialTime;

    public:
        Floater(GameScene& gameScene);
        virtual ~Floater() {}

        virtual void update(std::chrono::steady_clock::time_point curTime) override;
        virtual void render(Renderer& renderer) override;

#pragma pack(push, 1)
        struct ConfigStruct
        {
            sf::Vector2f position;
        };

        static_assert(sizeof(ConfigStruct) == sizeof(sf::Vector2f), "Packing failed!");
#pragma pack(pop)

        bool configure(const ConfigStruct& config);
    };
}
