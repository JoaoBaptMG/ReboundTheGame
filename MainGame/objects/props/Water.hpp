#pragma once

#include "objects/GameObject.hpp"
#include "objects/Player.hpp"

#include "drawables/WaterBody.hpp"

#include <SFML/Graphics.hpp>
#include <chipmunk/chipmunk.h>
#include <chrono>

class GameScene;
class Renderer;

namespace props
{
    class Water final : public GameObject
    {
        cpBB rect;
        WaterBody shape;

        cpFloat oldArea;

    public:
        Water(GameScene& scene);
        virtual ~Water();

        void setRect(sf::IntRect rect);

        auto getPosition() { return cpVect{rect.l, rect.b}; }
        auto getDisplayPosition()
        {
            auto pos = getPosition();
            return sf::Vector2f((float)std::floor(pos.x), (float)std::floor(pos.y));
        }
        
        virtual void update(std::chrono::steady_clock::time_point curTime) override;
        virtual void render(Renderer& renderer) override;
        
        virtual bool notifyScreenTransition(cpVect displacement) override;
        
    #pragma pack(push, 1)
        struct ConfigStruct
        {
            sf::Vector2<int16_t> position;
            int16_t width, height;
        };

        static_assert(sizeof(ConfigStruct) == 8*sizeof(char), "Packing failed!");
    #pragma pack(pop)

        bool configure(const ConfigStruct& config);
    };
}
