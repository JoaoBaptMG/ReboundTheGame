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

#include "PushableCrate.hpp"

#include "defaults.hpp"
#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include <vector_math.hpp>

#include <functional>
#include <limits>
#include <cppmunk/Body.h>
#include <cppmunk/PolyShape.h>
#include <cppmunk/Space.h>
#include <cppmunk/Arbiter.h>

#include "objects/GameObjectFactory.hpp"

using namespace props;
using namespace cp;

auto generateCrateData(size_t width, size_t height)
{
    util::grid<uint8_t> crateData(width / DefaultTileSize, height / DefaultTileSize);
    
    crateData(0, 0) = 0;
    crateData(crateData.width()-1, 0) = 2;
    crateData(0, crateData.height()-1) = 6;
    crateData(crateData.width()-1, crateData.height()-1) = 8;
    
    for (size_t i = 1; i < crateData.width()-1; i++)
    {
        crateData(i, 0) = 1;
        crateData(i, crateData.height()-1) = 7;
    }    
    
    for (size_t j = 1; j < crateData.height()-1; j++)
    {
        crateData(0, j) = 3;
        crateData(crateData.width()-1, j) = 5;
    }    
    
    for (size_t j = 1; j < crateData.height()-1; j++)
        for (size_t i = 1; i < crateData.width()-1; i++)
            crateData(i, j) = 4;
    
    return crateData;
}

void PushableCrate::setupCollisionHandlers(cp::Space* space)
{
    static Space* lastSpaceSetup = nullptr;

    if (lastSpaceSetup != space)
        lastSpaceSetup = space;
}

PushableCrate::PushableCrate(GameScene& gameScene) : GameObject(gameScene)
{
    sf::FloatRect drawingFrame{(float)(ScreenWidth-PlayfieldWidth)/2, (float)(ScreenHeight-PlayfieldHeight)/2,
        (float)PlayfieldWidth, (float)PlayfieldHeight};
    tilemap.setDrawingFrame(drawingFrame);
    
    tilemap.setTexture(gameScene.getResourceManager().load<sf::Texture>("push-crate.png"));
}

bool PushableCrate::configure(const PushableCrate::ConfigStruct& config)
{
    size_t width = DefaultTileSize * ((config.width + DefaultTileSize - 1)/DefaultTileSize);
    size_t height = DefaultTileSize * ((config.height + DefaultTileSize - 1)/DefaultTileSize);
    
    tilemap.setTileData(generateCrateData(width, height));
    setupPhysics(config.width, config.height);
    
    belongingRoomID = gameScene.getCurrentRoomID();
    
    if (gameScene.getLevelPersistentData().existsDataOfType<cpFloat>(getRotationKey()))
        shape->getBody()->setAngle(gameScene.getLevelPersistentData().getData<cpFloat>(getRotationKey()));
    if (gameScene.getLevelPersistentData().existsDataOfType<cpVect>(getPositionKey()))
        setPosition(gameScene.getLevelPersistentData().getData<cpVect>(getPositionKey()));
    else setPosition(cpVect{config.position.x - (cpFloat)width/2, config.position.y - (cpFloat)height/2});

    return true;
}

void PushableCrate::setupPhysics(float width, float height)
{
    setupCollisionHandlers(&gameScene.getGameSpace());

    auto body = std::make_shared<Body>(0.0, 0.0);
    shape = std::make_shared<PolyShape>(body, std::vector<cpVect>
        { cpVect{16, 16}, cpVect{width - 16, 16},
          cpVect{width - 16, height - 16}, cpVect{16, height - 16} }, 16);
    
    shape->setDensity(1);
    shape->setElasticity(0.6);
    shape->setFriction(0.4);
    shape->setCollisionType(CollisionType);

    gameScene.getGameSpace().add(body);
    gameScene.getGameSpace().add(shape);

    body->setUserData((void*)this);
}

PushableCrate::~PushableCrate()
{
    if (!transitionState)
    {
        gameScene.getLevelPersistentData().setData(getPositionKey(), getPosition());
        gameScene.getLevelPersistentData().setData(getRotationKey(), shape->getBody()->getAngle());
    }
    
    if (shape)
    {
        auto body = shape->getBody();
        
        gameScene.getGameSpace().remove(shape);
        gameScene.getGameSpace().remove(body);
    }
}

void PushableCrate::update(FrameTime curTime)
{
    auto player = gameScene.getObjectByName<Player>("player");
    
    if (player)
    {
		auto body = shape->getBody();

        if (player->canPushCrates())
        {
            auto vel = body->getVelocity();

            body->setBodyType(CP_BODY_TYPE_DYNAMIC);
			body->eachArbiter([](cp::Arbiter arbiter)
			{
				printf("Arbiter!\n");
			});
        }
		else
		{
			body->setBodyType(CP_BODY_TYPE_KINEMATIC);
		}
    }
}

bool PushableCrate::notifyScreenTransition(cpVect displacement)
{
    gameScene.getLevelPersistentData().setData(getPositionKey(), getPosition());
    gameScene.getLevelPersistentData().setData(getRotationKey(), shape->getBody()->getAngle());
    shape->getBody()->setPosition(shape->getBody()->getPosition() + displacement);
    return true;
}

void PushableCrate::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(getDisplayPosition());
    renderer.currentTransform.rotate(radiansToDegrees(shape->getBody()->getAngle()));
    renderer.pushDrawable(tilemap, {}, 25);
    renderer.popTransform();
}

REGISTER_GAME_OBJECT(props::PushableCrate);
