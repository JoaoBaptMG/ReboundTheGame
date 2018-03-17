#pragma once

#include "objects/GameObject.hpp"
#include "scene/GameScene.hpp"

#include <vector>
#include <chrono>

class GameScene;
class Renderer;

namespace props
{
    class SwitchingBlock;

    class SwitchingBlockCluster final : public ::GameObject
    {
        std::vector<std::vector<SwitchingBlock*>> blockTimes;
        std::chrono::steady_clock::time_point initTime;
        std::chrono::steady_clock::duration duration;
        intmax_t prevNumber;
        size_t numVisibleTimes;

    public:
        SwitchingBlockCluster(GameScene& scene);
        virtual ~SwitchingBlockCluster() = default;

        virtual void update(std::chrono::steady_clock::time_point curTime) override;
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