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

#include "SwitchingBlock.hpp"

#include "SwitchingBlockCluster.hpp"

#include "scene/GameScene.hpp"
#include "resources/ResourceManager.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Texture.hpp"

#include <cppmunk/Body.h>

#include <chronoUtils.hpp>
#include <streamReaders.hpp>

#include "objects/GameObjectFactory.hpp"

using namespace props;

FrameDuration SwitchingBlock::getFadeDuration()
{
    return 20_frames;
}

SwitchingBlock::SwitchingBlock(GameScene &scene) : GameObject(scene),
    blockSprite(scene.getResourceManager().load<Texture>("switching-block.png")),
    fadeSprite(scene.getResourceManager().load<Texture>("switching-block-fade.png")),
    visible(false), blockClusterName(), blockTime(0), parentBlockCluster(nullptr),
    fadeTime(), curTime()
{

}

bool props::readFromStream(InputStream& stream, SwitchingBlock::ConfigStruct& config)
{
    return ::readFromStream(stream, config.position, config.blockClusterName, varLength(config.blockTime));
}

bool SwitchingBlock::configure(const SwitchingBlock::ConfigStruct& config)
{
    blockClusterName = config.blockClusterName;
    blockTime = config.blockTime;

    setupPhysics();
    setPosition(cpVect{(cpFloat)config.position.x, (cpFloat)config.position.y});

    return true;
}

void SwitchingBlock::setupPhysics()
{
    auto body = std::make_shared<cp::Body>(cp::Body::Kinematic);

    blockShape = std::make_shared<cp::PolyShape>(body, std::vector<cpVect>
        { cpVect{-24, -24}, cpVect{+24, -24}, cpVect{+24, +24}, cpVect{-24, +24} }, 8);

    blockShape->setDensity(1);
    blockShape->setElasticity(0.6);
    blockShape->setFriction(0.4);
}

SwitchingBlock::~SwitchingBlock()
{
    if (blockShape)
    {
        auto body = blockShape->getBody();

        if (cpSpaceContainsBody(gameScene.getGameSpace(), *body))
        {
            gameScene.getGameSpace().remove(blockShape);
            gameScene.getGameSpace().remove(body);
        }
    }

    if (parentBlockCluster != nullptr)
    {
        parentBlockCluster = gameScene.getObjectByName<SwitchingBlockCluster>(blockClusterName);
        if (parentBlockCluster != nullptr) parentBlockCluster->unregisterSwitchBlock(blockTime, this);
    }
}

void SwitchingBlock::update(FrameTime curTime)
{
    this->curTime = curTime;

    if (parentBlockCluster == nullptr)
    {
        parentBlockCluster = gameScene.getObjectByName<SwitchingBlockCluster>(blockClusterName);
        if (parentBlockCluster == nullptr) remove();
        else parentBlockCluster->registerSwitchBlock(blockTime, this);
    }

    float fadeFactor = 2 * toSeconds<float>(curTime - fadeTime) / toSeconds<float>(getFadeDuration());
    if (fadeFactor > 1) fadeFactor = 2 - fadeFactor;
    if (fadeFactor == 0) fadeFactor = 0;
    fadeSprite.setOpacity(fadeFactor);
}

void SwitchingBlock::switchOn()
{
    auto body = blockShape->getBody();
    if (!cpSpaceContainsBody(gameScene.getGameSpace(), *body))
    {
        gameScene.getGameSpace().add(blockShape);
        gameScene.getGameSpace().add(body);
    }
    visible = true;
}

void SwitchingBlock::switchOff()
{
    auto body = blockShape->getBody();
    if (cpSpaceContainsBody(gameScene.getGameSpace(), *body))
    {
        gameScene.getGameSpace().remove(blockShape);
        gameScene.getGameSpace().remove(body);
    }
    visible = false;
}

void SwitchingBlock::doFade()
{
    fadeTime = curTime;
}

bool SwitchingBlock::notifyScreenTransition(cpVect displacement)
{
    setPosition(getPosition() + displacement);
    return true;
}

void SwitchingBlock::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform *= util::translate(getDisplayPosition());
    if (visible) renderer.pushDrawable(blockSprite, 36);
    if (fadeSprite.getOpacity() > 0) renderer.pushDrawable(fadeSprite, 37);
    renderer.popTransform();
}

REGISTER_GAME_OBJECT(props::SwitchingBlock);
