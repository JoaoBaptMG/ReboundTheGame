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
    {
        space->addCollisionHandler(Player::CollisionType, CollisionType,
            [](Arbiter arbiter, Space& space)
            {
                auto player = static_cast<Player*>(cpBodyGetUserData(arbiter.getBodyA()));
                auto crate = static_cast<PushableCrate*>(cpBodyGetUserData(arbiter.getBodyB()));
                return cpArbiterCallWildcardBeginA(arbiter, space);
            }, cpArbiterCallWildcardPreSolveA, cpArbiterCallWildcardPostSolveA, cpArbiterCallWildcardSeparateA);
        
        lastSpaceSetup = space;
    }
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
    if (gameScene.getLevelPersistentData().existsDataOfType<cpVect>(getPositionKey()))
        setPosition(gameScene.getLevelPersistentData().getData<cpVect>(getPositionKey()));
    else setPosition(cpVect{config.position.x - (cpFloat)width/2, config.position.y - (cpFloat)height/2});
    
    if (gameScene.getLevelPersistentData().existsDataOfType<cpFloat>(getRotationKey()))
        shape->getBody()->setAngle(gameScene.getLevelPersistentData().getData<cpFloat>(getRotationKey()));

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

void PushableCrate::update(std::chrono::steady_clock::time_point curTime)
{
    auto player = gameScene.getObjectByName<Player>("player");
    
    if (player)
    {
        if (player->canPushCrates())
        {
            auto body = shape->getBody();
            auto vel = body->getVelocity();

            body->setBodyType(CP_BODY_TYPE_DYNAMIC);

            if (std::abs(vel.x) < 1.0)
            {
                vel.x = 0;
                body->setVelocity(vel);
            }
            else
            {
                auto sgn = vel.x > 0 ? -1.0 : vel.x < 0 ? 1.0 : 0.0;
                body->applyForceAtLocalPoint(cpVect{ 18 * sgn * body->getMass(), 0 }, cpVect{0, 0});
            }
        }
        else shape->getBody()->setBodyType(CP_BODY_TYPE_KINEMATIC);
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

