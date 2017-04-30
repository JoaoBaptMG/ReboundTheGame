#include "PushableCrate.hpp"

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "utility/vector_math.hpp"

#include <functional>
#include <limits>
#include <cppmunk/Body.h>
#include <cppmunk/PolyShape.h>
#include <cppmunk/Space.h>
#include <cppmunk/Arbiter.h>

using namespace props;
using namespace cp;

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
                if (player->canPushCrates())
                {
                    arbiter.setRestitution(0);
                    space.addPostStepCallback(nullptr, [=]
                    {
                        crate->shape->getBody()->setBodyType(CP_BODY_TYPE_DYNAMIC);
                    });
                }
                else space.addPostStepCallback(nullptr, [=]
                {
                    crate->shape->getBody()->setBodyType(CP_BODY_TYPE_KINEMATIC);
                });
                return true;
            }, [](Arbiter, Space&) { return true; },
            [](Arbiter, Space&) {}, [](Arbiter arbiter, Space&) {});
        
        lastSpaceSetup = space;
    }
}

PushableCrate::PushableCrate(GameScene& gameScene) : GameObject(gameScene)
{
    rect.setFillColor(sf::Color(180, 100, 0));
    rect.setOutlineColor(sf::Color::Black);
    rect.setOutlineThickness(4);
}

bool PushableCrate::configure(const PushableCrate::ConfigStruct& config)
{
    rect.setSize(sf::Vector2f((float)config.width, (float)config.height));
    rect.setOrigin(sf::Vector2f((float)config.width/2, (float)config.height/2));

    setupPhysics(config.width, config.height);
    setPosition(cpVect{(cpFloat)config.position.x, (cpFloat)config.position.y});

    return true;
}

void PushableCrate::setupPhysics(float width, float height)
{
    setupCollisionHandlers(&gameScene.getGameSpace());

    auto body = std::make_shared<Body>(0.0, 0.0);
    shape = std::make_shared<PolyShape>(body, std::vector<cpVect>
        { cpVect{-width/2, -height/2}, cpVect{+width/2, -height/2},
          cpVect{+width/2, +height/2}, cpVect{-width/2, +height/2} });
    
    shape->setDensity(1);
    shape->setElasticity(0.6);
    shape->setFriction(0.4);
    shape->setCollisionType(CollisionType);

    gameScene.getGameSpace().add(body);
    gameScene.getGameSpace().add(shape);

    body->setMoment(std::numeric_limits<cpFloat>::infinity());
    body->setUserData((void*)this);
}

PushableCrate::~PushableCrate()
{
    if (shape)
    {
        auto body = shape->getBody();
        gameScene.getGameSpace().remove(shape);
        gameScene.getGameSpace().remove(body);
    }
}

void PushableCrate::update(std::chrono::steady_clock::time_point curTime)
{
    auto body = shape->getBody();
    auto vel = body->getVelocity();

    if (vel.x < 0.1)
    {
        vel.x = 0;
        body->setVelocity(vel);
    }
    else
    {
        auto sgn = vel.x > 0 ? -1.0 : vel.x < 0 ? 1.0 : 0.0;
        body->applyForceAtLocalPoint(cpVect{ sgn * body->getMass(), 0 }, cpVect{0, 0});
    }
}

void PushableCrate::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(getDisplayPosition());
    renderer.pushDrawable(rect, {}, 25);
    renderer.popTransform();
}
