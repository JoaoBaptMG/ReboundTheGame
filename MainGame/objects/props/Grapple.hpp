#pragma once

#include "objects/GameObject.hpp"
#include "drawables/Sprite.hpp"

#include <chipmunk/chipmunk.h>
#include <chrono>

class GameScene;
class Renderer;

namespace props
{
    class Grapple final : public ::GameObject
    {
        Sprite sprite, grappleSprite;
        
        cpVect pos;
        float lastFade;
        bool isExcited;
        std::chrono::steady_clock::time_point curTime, initialTime, exciteTime;
    
    public:
        Grapple(GameScene& gameScene);
        virtual ~Grapple();

        virtual void update(std::chrono::steady_clock::time_point curTime) override;
        virtual void render(Renderer& renderer) override;
        virtual bool notifyScreenTransition(cpVect displacement) override;

        auto getDisplayPosition() const
        {
            return sf::Vector2f(std::floor(pos.x), std::floor(pos.y));
        }

        #pragma pack(push, 1)
        struct ConfigStruct
        {
            sf::Vector2<int16_t> position;
        };

        static_assert(sizeof(ConfigStruct) == 4*sizeof(char), "Packing failed!");
#pragma pack(pop)

        bool configure(const ConfigStruct& config);
    };
}
