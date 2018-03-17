#include "SwitchingBlock.hpp"

#include "SwitchingBlockCluster.hpp"

#include "scene/GameScene.hpp"
#include "resources/ResourceManager.hpp"
#include "rendering/Renderer.hpp"

#include <cppmunk/Body.h>

#include <chronoUtils.hpp>
#include <streamReaders.hpp>

using namespace props;

SwitchingBlock::SwitchingBlock(GameScene &scene) : GameObject(scene),
    blockSprite(scene.getResourceManager().load<sf::Texture>("switching-block.png")),
    visible(false), blockClusterName(), blockTime(0), parentBlockCluster(nullptr),
    switchTime(), curTime()
{

}

bool props::readFromStream(sf::InputStream& stream, SwitchingBlock::ConfigStruct& config)
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

void SwitchingBlock::update(std::chrono::steady_clock::time_point curTime)
{
    this->curTime = curTime;

    if (parentBlockCluster == nullptr)
    {
        parentBlockCluster = gameScene.getObjectByName<SwitchingBlockCluster>(blockClusterName);
        if (parentBlockCluster == nullptr) remove();
        else parentBlockCluster->registerSwitchBlock(blockTime, this);
    }
}

void SwitchingBlock::switchOn()
{
    switchTime = curTime;

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
    switchTime = curTime;

    auto body = blockShape->getBody();
    if (cpSpaceContainsBody(gameScene.getGameSpace(), *body))
    {
        gameScene.getGameSpace().remove(blockShape);
        gameScene.getGameSpace().remove(body);
    }
    visible = false;
}

bool SwitchingBlock::notifyScreenTransition(cpVect displacement)
{
    setPosition(getPosition() + displacement);
    return true;
}

void SwitchingBlock::render(Renderer& renderer)
{
    if (visible)
    {
        renderer.pushTransform();
        renderer.currentTransform.translate(getDisplayPosition());
        renderer.pushDrawable(blockSprite, {}, 36);
        renderer.popTransform();
    }
}