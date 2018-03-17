#include "SwitchingBlockCluster.hpp"

#include "SwitchingBlock.hpp"

#include <chronoUtils.hpp>
#include <algorithm>
#include <streamReaders.hpp>

#include "defaults.hpp"

#include "scene/GameScene.hpp"

using namespace props;

SwitchingBlockCluster::SwitchingBlockCluster(GameScene& scene) : GameObject(scene),
    prevNumber(0), numVisibleTimes(0)
{

}

bool props::readFromStream(sf::InputStream& stream, SwitchingBlockCluster::ConfigStruct& config)
{
    return ::readFromStream(stream, config.position, varLength(config.numBlockTimes),
        varLength(config.numVisibleTimes), config.durationSeconds);
}

bool SwitchingBlockCluster::configure(const SwitchingBlockCluster::ConfigStruct& config)
{
    using namespace std::chrono;

    blockTimes.resize(config.numBlockTimes);
    numVisibleTimes = config.numVisibleTimes;
    duration = duration_cast<decltype(duration)>(FloatSeconds(config.durationSeconds));

    return true;
}

void SwitchingBlockCluster::registerSwitchBlock(size_t id, SwitchingBlock* block)
{
    if (id >= blockTimes.size()) return;
    blockTimes.at(id).push_back(block);
}

void SwitchingBlockCluster::unregisterSwitchBlock(size_t id, SwitchingBlock* block)
{
    if (id >= blockTimes.size()) return;
    blockTimes.at(id).erase(std::remove(blockTimes.at(id).begin(), blockTimes.at(id).end(), block), blockTimes.at(id).end());
}

void SwitchingBlockCluster::update(std::chrono::steady_clock::time_point curTime)
{
    if (isNull(initTime))
    {
        initTime = curTime + 4*duration;
        prevNumber = -4;
    }

    if (curTime < initTime) return;

    intmax_t curNumber = (curTime - initTime) / duration;
    if (curNumber != prevNumber)
    {
        size_t appearId = curNumber % blockTimes.size();
        size_t disappearId = (curNumber - numVisibleTimes + blockTimes.size()) % blockTimes.size();

        for (auto block : blockTimes.at(appearId)) block->switchOn();
        for (auto block : blockTimes.at(disappearId)) block->switchOff();
    }

    prevNumber = curNumber;
}