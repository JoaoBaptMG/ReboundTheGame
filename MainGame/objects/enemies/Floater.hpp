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
    class Floater final : public ::Enemy
    {
        cpVect originalPos;
        Sprite sprite;

        std::chrono::steady_clock::time_point initialTime;
        std::shared_ptr<cp::Shape> collisionShapes[4];

    public:
        Floater(GameScene& gameScene);
        virtual ~Floater();

        void setupPhysics();

        virtual bool onCollisionAttack(Player& player, std::shared_ptr<cp::Shape> shape) override;
        virtual bool onCollisionHit(Player& player, std::shared_ptr<cp::Shape> shape) override;

        virtual void update(std::chrono::steady_clock::time_point curTime) override;
        virtual void render(Renderer& renderer) override;
        
        virtual bool notifyScreenTransition(cpVect displacement) override;

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
