//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

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

void SwitchingBlockCluster::update(FrameTime curTime)
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
