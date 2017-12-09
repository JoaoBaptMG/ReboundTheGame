#pragma once

#include "objects/Enemy.hpp"
#include "drawables/Sprite.hpp"

#include <SFML/Graphics.hpp>
#include <chrono>
#include <memory>
#include <cppmunk/Body.h>
#include <cppmunk/Shape.h>

class GameScene;
class Renderer;
class Player;

namespace enemies
{
    class Rotator final : public ::Enemy
    {
        cpBB wanderingArea;

        Sprite sprite;
        std::shared_ptr<cp::Shape> collisionShapes[3];

    public:
        Rotator(GameScene& gameScene);
        virtual ~Rotator();

        void setupPhysics();

        virtual bool onCollisionAttack(Player& player) override;
        virtual bool onCollisionHit(Player& player) override;

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
