#include "SwitchingBlockCluster.hpp"

#include "SwitchingBlock.hpp"

#include <chronoUtils.hpp>
#include <algorithm>
#include <streamReaders.hpp>

#include "defaults.hpp"

#include "scene/GameScene.hpp"

#include <utility>
#include <iostream>

#include "objects/GameObjectFactory.hpp"

using namespace props;

SwitchingBlockCluster::SwitchingBlockCluster(GameScene& scene) : GameObject(scene),
    prevNumber(0), numVisibleTimes(0), fadeRequested(false)
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

template <typename T, typename U>
auto divModTruncated(const T& dividend, const U& divisor)
{
    auto quotient = dividend / divisor;
    auto remainder = dividend % divisor;
    if (remainder <= decltype(remainder)(0))
    {
        quotient -= 1;
        remainder += divisor;
    }
    return std::make_pair(quotient, remainder);
};

void SwitchingBlockCluster::update(std::chrono::steady_clock::time_point curTime)
{
    if (isNull(initTime))
    {
        initTime = curTime + 4*duration;
        prevNumber = -1;
    }

    auto halfDuration = SwitchingBlock::getFadeDuration()/2;

    if (curTime < initTime - halfDuration) return;
    auto pair = divModTruncated(curTime - initTime, duration);

    intmax_t curNumber = pair.first;
    if (curNumber != prevNumber)
    {
        size_t appearId = curNumber % blockTimes.size();
        size_t disappearId = (curNumber - numVisibleTimes + blockTimes.size()) % blockTimes.size();

        for (auto block : blockTimes.at(appearId)) block->switchOn();
        for (auto block : blockTimes.at(disappearId)) block->switchOff();

        fadeRequested = false;
    }
    if (!fadeRequested && pair.second >= duration - halfDuration)
    {
        size_t appearId = (curNumber + 1) % blockTimes.size();
        size_t disappearId = (curNumber - numVisibleTimes + blockTimes.size() + 1) % blockTimes.size();

        for (auto block : blockTimes.at(appearId)) block->doFade();
        if (prevNumber >= 0) for (auto block : blockTimes.at(disappearId)) block->doFade();

        fadeRequested = true;
    }

    prevNumber = curNumber;
}

REGISTER_GAME_OBJECT(props::SwitchingBlockCluster);
