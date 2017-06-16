#pragma once

#include "objects/Collectible.hpp"
#include "drawables/Sprite.hpp"

#include <SFML/Graphics.hpp>
#include <chrono>
#include <memory>
#include <cppmunk/Body.h>
#include <cppmunk/Shape.h>

namespace collectibles
{
    class HealthPickup final : public ::Collectible
    {
        Sprite sprite;
        std::shared_ptr<cp::Shape> collisionShape;
        size_t healthAmount;

    public:
        HealthPickup(GameScene& scene, size_t amount);
        HealthPickup(GameScene& scene);
        virtual ~HealthPickup();

        void setupPhysics();

        virtual void onCollect(Player& player) override;

        virtual void update(std::chrono::steady_clock::time_point curTime) override;
        virtual void render(Renderer& renderer) override;
    
#pragma pack(push, 1)
        struct ConfigStruct
        {
            sf::Vector2<int16_t> position;
            int32_t healthAmount;
        };

        static_assert(sizeof(ConfigStruct) == 8*sizeof(char), "Packing failed!");
#pragma pack(pop)

        bool configure(const ConfigStruct& config);
    };
}
