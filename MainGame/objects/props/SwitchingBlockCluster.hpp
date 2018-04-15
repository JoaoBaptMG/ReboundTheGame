#pragma once

#include "objects/GameObject.hpp"
#include "scene/GameScene.hpp"

#include <vector>
#include <chronoUtils.hpp>

class GameScene;
class Renderer;

namespace props
{
    class SwitchingBlock;

    class SwitchingBlockCluster final : public ::GameObject
    {
        std::vector<std::vector<SwitchingBlock*>> blockTimes;
        FrameTime initTime;
        FrameDuration duration;
        intmax_t prevNumber; bool fadeRequested;
        size_t numVisibleTimes;

    public:
        SwitchingBlockCluster(GameScene& scene);
        virtual ~SwitchingBlockCluster() = default;

        virtual void update(FrameTime curTime) override;
        virtual void render(Renderer& renderer) override {}

        void registerSwitchBlock(size_t id, SwitchingBlock* block);
        void unregisterSwitchBlock(size_t id, SwitchingBlock* block);

        struct ConfigStruct
        {
            sf::Vector2<int16_t> position;
            size_t numBlockTimes, numVisibleTimes;
            float durationSeconds;
        };

        bool configure(const ConfigStruct& config);
    };

    bool readFromStream(sf::InputStream& stream, SwitchingBlockCluster::ConfigStruct& config);
}