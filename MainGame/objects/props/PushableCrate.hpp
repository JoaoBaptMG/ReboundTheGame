#pragma once

#include <memory>

#include <chipmunk/chipmunk.h>
#include <cppmunk/Shape.h>
#include <cppmunk/Body.h>

#include "objects/GameObject.hpp"
#include "scene/GameScene.hpp"
#include "drawables/Tilemap.hpp"

class GameScene;
class Renderer;

namespace props
{
    class PushableCrate final : public ::GameObject
    {
        static void setupCollisionHandlers(cp::Space* space);

        Tilemap tilemap;
        std::shared_ptr<cp::Shape> shape;
        size_t belongingRoomID;

    public:
        PushableCrate(GameScene& scene);
        virtual ~PushableCrate();

        void setupPhysics(float width, float height);

        virtual void update(std::chrono::steady_clock::time_point curTime) override;
        virtual void render(Renderer& renderer) override;
        
        virtual bool notifyScreenTransition(cpVect displacement) override;

        auto getPosition() const { return shape->getBody()->getPosition(); }
        void setPosition(cpVect pos) { shape->getBody()->setPosition(pos); }

        auto getDisplayPosition() const
        {
            auto vec = getPosition();
            return sf::Vector2f((float)std::round(vec.x), (float)std::round(vec.y));
        }

        auto getPositionKey() const
        {
            return std::to_string(belongingRoomID) + "." + getName() + ".position";
        }
        
        auto getRotationKey() const
        {
            return std::to_string(belongingRoomID) + "." + getName() + ".rotation";
        }

#pragma pack(push, 1)
        struct ConfigStruct
        {
            sf::Vector2<int16_t> position;
            int16_t width, height;
        };

        static_assert(sizeof(ConfigStruct) == 8*sizeof(char), "Packing failed!");
#pragma pack(pop)

        bool configure(const ConfigStruct& config);

        static constexpr cpCollisionType CollisionType = 'cpsh';
    };
}
