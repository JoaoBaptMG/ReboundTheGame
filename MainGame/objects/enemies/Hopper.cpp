#include "Hopper.hpp"

#include "objects/GameObjectFactory.hpp"

#include "misc/ShapeUtils.hpp"
#include "scene/GameScene.hpp"
#include "resources/ResourceManager.hpp"
#include "rendering/Renderer.hpp"
#include "particles/TextureExplosion.hpp"

#include <streamReaders.hpp>
#include <cppmunk/CircleShape.h>
#include <cppmunk/SegmentShape.h>
#include <cppmunk/PolyShape.h>
#include <cppmunk/DampedSpring.h>
#include <cppmunk/Arbiter.h>
#include <cpVectOperations.h>

#include <vector_math.hpp>
#include <iostream>

using namespace enemies;

bool enemies::readFromStream(sf::InputStream& stream, Hopper::ConfigStruct& config)
{
    uint8_t faceRight;
    if (!::readFromStream(stream, config.position, faceRight)) return false;
    config.facingRight = faceRight;
    return true;
}

Hopper::Hopper(GameScene& gameScene) : EnemyCommon(gameScene), facingRight(false),
    hopperBody(gameScene.getResourceManager().load<sf::Texture>("hopper-body.png")),
    hopperLeg(gameScene.getResourceManager().load<sf::Texture>("hopper-leg.png"), sf::Vector2f(55, 5)),
    hopperFoot(gameScene.getResourceManager().load<sf::Texture>("hopper-foot.png"))
{
    setHealth(3);
    setTouchDamage(3);
    setDashDamage(3);
    setBombDamage(1);
}

bool Hopper::configure(const Hopper::ConfigStruct& config)
{
    setupPhysics(cpVect{(cpFloat)config.position.x, (cpFloat)config.position.y});
    setFacingDirection(config.facingRight);
    return true;
}

void Hopper::setupPhysics(cpVect position)
{
    mainBody = std::make_shared<cp::Body>(0, 0);
    footBody = std::make_shared<cp::Body>(0, 0);
    
    mainShape[0] = std::make_shared<cp::CircleShape>(mainBody, 10, cpv(40, 16));
    mainShape[1] = std::make_shared<cp::PolyShape>(mainBody, std::vector<cpVect>
        { cpv(40, 26), cpv(-24, 26), cpv(-29.0283, 25.5091),cpv(-33.8668, 24.0551),
            cpv(-38.3327, 21.6927), cpv(-42.2574, 18.5113), cpv(-45.4927, 14.6309), cpv(-47.9165, 10.198),
            cpv(-49.4373, 5.3801), cpv(-49.9975, 0.3591), cpv(-49.5761, -4.6756), cpv(-48.189, -9.5336),
            cpv(-45.8886, -14.0317), cpv(-42.7617, -18), cpv(-2.5, -18), cpv(44.7059, 7.1765) }, 0);
    
    footShape = std::make_shared<cp::PolyShape>(footBody, std::vector<cpVect>
        { cpv(-10, 8), cpv(-6, -8), cpv(6, -8), cpv(10, 8) }, 0);
    
    damageShape = std::make_shared<cp::PolyShape>(mainBody, std::vector<cpVect>
        { cpv(-42.7617, -18), cpv(-38.4676, -21.603), cpv(-33.5011, -24.2019),
            cpv(-28.0929, -25.6758), cpv(-22.4946, -25.9564), cpv(-16.9662, -25.0305),
            cpv(-11.7647, -22.9412), cpv(-2.5, -18)}, 0);
    
    for (auto shptr : { &mainShape[0], &mainShape[1], &footShape, &damageShape })
    {
        auto& shape = *shptr;

        shape->setCollisionType(Enemy::GroundStandCollisionType);
        shape->setElasticity(1.0);
        shape->setFriction(0.0);
        shape->setDensity(2.5);
        
        shape->setFilter({ reinterpret_cast<cpGroup>(this), CP_ALL_CATEGORIES, CP_ALL_CATEGORIES });
        
        gameScene.getGameSpace().add(shape);
    }
    
    damageShape->setCollisionType(Enemy::GroundStandHitCollisionType);
    
    mainBody->setPosition(position);
    footBody->setPosition(position + cpv(40, 80));
    
    mainBody->setMoment(std::numeric_limits<cpFloat>::infinity());
    footBody->setMoment(std::numeric_limits<cpFloat>::infinity());
    
    for (const auto& body : { mainBody, footBody })
        gameScene.getGameSpace().add(body);
    
    collisionBody = mainBody;
    footBody->setUserData((void*)this);
    
    Enemy::setupPhysics();
}

Hopper::~Hopper()
{
    for (const auto& shape : { mainShape[0], mainShape[1], footShape, damageShape })
        gameScene.getGameSpace().remove(shape);
    for (const auto& body : { mainBody, footBody })
        gameScene.getGameSpace().remove(body);
}

size_t Hopper::playerDamage(Player& player, const std::shared_ptr<cp::Shape> shape) const
{
    return 24;
}

void Hopper::die()
{
    EnemyCommon::die();
    
    auto roundVec = [](cpVect vec) { return sf::Vector2f(round(vec.x), round(vec.y)); };
    
    auto grav = gameScene.getGameSpace().getGravity();
    auto displayGravity = sf::Vector2f(grav.x, grav.y);
    
    auto explosion = std::make_unique<TextureExplosion>(gameScene, hopperBody.getTexture(), 100 * UpdatePeriod,
        sf::FloatRect(-80, -32, 160, 16), displayGravity, TextureExplosion::Density, 4, 4, 160);
    explosion->setPosition(roundVec(mainBody->getPosition()));
    gameScene.addObject(std::move(explosion));
}

void Hopper::update(std::chrono::steady_clock::time_point curTime)
{
    EnemyCommon::update(curTime);
    
    auto diff = footBody->getPosition().y - mainBody->getPosition().y;
    auto mvel = mainBody->getVelocity(), fvel = footBody->getVelocity();
    if (diff < 80)
    {
        std::cout << mvel.y << std::endl;
        auto imp = std::max<cpFloat>(-580 - mvel.y, -7200 * toSeconds<cpFloat>(UpdatePeriod));
        imp = std::max<cpFloat>(imp, (diff - 80) / toSeconds<cpFloat>(UpdatePeriod));
        mainBody->applyForceAtLocalPoint(-mainBody->getMass() * gameScene.getGameSpace().getGravity(), cpvzero);
        mainBody->applyImpulseAtLocalPoint(mainBody->getMass() * cpv(0, imp), cpvzero);
    }
    else if (cpvlengthsq(footBody->getVelocity()) < 12 && mvel.y < 0)
        footBody->applyImpulseAtLocalPoint(footBody->getMass() * mainBody->getVelocity(), cpvzero);
    
    mainBody->applyImpulseAtLocalPoint(mainBody->getMass() * cpv(-mvel.x, 0), cpvzero);
    footBody->applyImpulseAtLocalPoint(mainBody->getMass() * cpv(-fvel.x, 0), cpvzero);
    
    graphicalDisplacement = cpvzero;
    footBody->eachArbiter([=](cp::Arbiter arbiter)
    {
        cpFloat sgn = cpShapeGetCollisionType(arbiter.getShapeA()) == GroundStandCollisionType ? 1 : -1;
        cpFloat depthSum = 0;
        
        auto set = cpArbiterGetContactPointSet(arbiter);
        for (size_t i = 0; i < set.count; i++)
            depthSum += set.points[i].distance;
        
        graphicalDisplacement += sgn * depthSum * set.normal;
    });
}

void Hopper::setFacingDirection(bool facingRight)
{
    if (facingRight != this->facingRight)
        for (auto shape : { &mainShape[0], &mainShape[1], &footShape, &damageShape })
            flipShapeHorizontally(*shape);
    
    this->facingRight = facingRight;
}

void Hopper::render(Renderer& renderer)
{
    auto roundVec = [](cpVect vec) { return sf::Vector2f(round(vec.x), round(vec.y)); };
    
    applyBlinkEffect(hopperBody);
    applyBlinkEffect(hopperLeg);
    applyBlinkEffect(hopperFoot);
    
    float dy = roundVec(mainBody->getPosition() - footBody->getPosition() - graphicalDisplacement).y + 16;
    float angle = radiansToDegrees(asinf(dy/(2*50)));
    
    renderer.pushTransform();
    renderer.currentTransform.translate(roundVec(mainBody->getPosition()));
    renderer.currentTransform.rotate(radiansToDegrees(mainBody->getAngle()));
    renderer.pushDrawable(hopperBody, {}, 47);
    renderer.currentTransform.translate(40, 16);
    renderer.currentTransform.rotate(angle);
    renderer.pushDrawable(hopperLeg, {}, 44);
    renderer.popTransform();
    
    renderer.pushTransform();
    renderer.currentTransform.translate(roundVec(footBody->getPosition() + graphicalDisplacement));
    renderer.currentTransform.rotate(radiansToDegrees(footBody->getAngle()));
    renderer.pushDrawable(hopperFoot, {}, 46);
    renderer.currentTransform.rotate(-angle);
    renderer.pushDrawable(hopperLeg, {}, 45);
    renderer.popTransform();
}


REGISTER_GAME_OBJECT(enemies::Hopper);
