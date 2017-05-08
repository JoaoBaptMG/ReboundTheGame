#pragma once

#include <memory>

#include <chipmunk/chipmunk.h>
#include <cppmunk/Shape.h>
#include <cppmunk/Body.h>

#include "objects/GameObject.hpp"
#include "scene/GameScene.hpp"
#include "drawables/Sprite.hpp"

class GameScene;
class Renderer;
class Bomb;

namespace props
{
    class BombCrate final : public ::GameObject
    {
        Sprite sprite;
        std::shared_ptr<cp::Shape> shape;
        std::function<void(Bomb*)> bombHandler;

    public:
        BombCrate(GameScene& scene);
        virtual ~BombCrate();

        void setupPhysics();

        virtual void update(std::chrono::steady_clock::time_point curTime) override;
        virtual void render(Renderer& renderer) override;

        auto getPosition() const { return shape->getBody()->getPosition(); }
        void setPosition(cpVect pos) { shape->getBody()->setPosition(pos); }

        auto getDisplayPosition() const
        {
            auto vec = getPosition();
            return sf::Vector2f(std::floor(vec.x), std::floor(vec.y));
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
