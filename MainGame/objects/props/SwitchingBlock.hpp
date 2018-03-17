#pragma once

#include "objects/GameObject.hpp"
#include "scene/GameScene.hpp"

#include "drawables/Sprite.hpp"

#include <vector>
#include <chrono>

#include <cppmunk/PolyShape.h>

class GameScene;
class Renderer;

namespace props
{
    class SwitchingBlockCluster;

    class SwitchingBlock final : public ::GameObject
    {
        Sprite blockSprite;
        std::shared_ptr<cp::Shape> blockShape;

        bool visible;
        std::string blockClusterName;
        size_t blockTime;
        SwitchingBlockCluster* parentBlockCluster;

        std::chrono::steady_clock::time_point switchTime, curTime;

        void setupPhysics();

    public:
        SwitchingBlock(GameScene& scene);
        virtual ~SwitchingBlock();

        virtual void update(std::chrono::steady_clock::time_point curTime) override;
        virtual void render(Renderer& renderer) override;

        virtual bool notifyScreenTransition(cpVect displacement) override;

        void switchOn();
        void switchOff();

        auto getPosition() const { return blockShape->getBody()->getPosition(); }
        void setPosition(cpVect pos) { blockShape->getBody()->setPosition(pos); }

        auto getDisplayPosition() const
        {
            auto vec = getPosition();
            return sf::Vector2f((float)std::round(vec.x), (float)std::round(vec.y));
        }

        struct ConfigStruct
        {
            sf::Vector2<int16_t> position;
            std::string blockClusterName;
            size_t blockTime;
        };

        bool configure(const ConfigStruct& config);
    };

    bool readFromStream(sf::InputStream& stream, SwitchingBlock::ConfigStruct& config);
}