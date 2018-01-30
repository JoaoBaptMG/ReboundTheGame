#include "TestBoss.hpp"

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "objects/Room.hpp"
#include "particles/TextureExplosion.hpp"

#include <cppmunk/Body.h>
#include <cppmunk/CircleShape.h>
#include <cppmunk/PolyShape.h>
#include <cppmunk/Arbiter.h>

#include <chronoUtils.hpp>

using namespace enemies;
using namespace enemies::bosses;

constexpr size_t MaxHealth = 12;
constexpr auto SpawnPeriod = 120 * UpdateFrequency;
constexpr auto ExplosionDuration = 200 * UpdateFrequency;

TestBoss::TestBoss(GameScene& scene) : Boss(scene, "test-boss-present"),
    mainSprite(scene.getResourceManager().load<sf::Texture>("test-boss.png"))
{
    setupPhysics();
    
    setBombDamage(1);
    setHealth(MaxHealth);
    
    collisionBody->setVelocity(cpVect{ 32, 0 });
}

bool TestBoss::configure(const TestBoss::ConfigStruct& config)
{
    collisionBody->setPosition({ (float)config.position.x, (float)config.position.y });
    leftSpan = config.leftSpan;
    rightSpan = config.rightSpan;
    return true;
}

TestBoss::~TestBoss()
{
    for (const auto& shape : collisionShapes)
        gameScene.getGameSpace().remove(shape);
    gameScene.getGameSpace().remove(collisionBody);
}

void TestBoss::setupPhysics()
{
    using namespace cp;
    collisionBody = std::make_shared<Body>(Body::Kinematic);
    
    for (size_t i : { 0, 1 })
    {
        collisionShapes[i] = std::make_shared<PolyShape>(collisionBody, std::vector<cpVect>
            { cpVect{ -48, -112 }, cpVect{  48, -112 }, cpVect{  80, -48 },
              cpVect{  48,   16 }, cpVect{ -48,   16 }, cpVect{ -80, -48 } });
    }
    collisionShapes[2] = std::make_shared<PolyShape>(collisionBody, std::vector<cpVect>
        { cpVect{ -16, 16 }, cpVect{ 16,  16 },
          cpVect{  16, 96 }, cpVect{ -16, 96 } }, 16);
    
    for (const auto& shape : collisionShapes)
    {
        shape->setCollisionType(Enemy::CollisionType);
        
        shape->setElasticity(1.0);
        shape->setFriction(0.0);
        gameScene.getGameSpace().add(shape);
    }
    
    addBombShape(collisionShapes[0], bombShapeHandler);
    gameScene.getGameSpace().add(collisionBody);
    
    Enemy::setupPhysics();
}

void TestBoss::update(std::chrono::steady_clock::time_point curTime)
{
    EnemyCommon::update(curTime);
    
    if (isNull(initTime)) initTime = curTime;
    
    auto pos = collisionBody->getPosition();
    auto vel = collisionBody->getVelocity();
    
    if (vel.x > 0 && pos.x > rightSpan-80)
        collisionBody->setVelocity(-vel);
    else if (vel.x < 0 && pos.x < leftSpan+80)
        collisionBody->setVelocity(-vel);
    
    if (curTime >= initTime + SpawnPeriod)
    {
        initTime = curTime;
        
        auto vel1 = cpVect{0, 160};
        auto vel2 = cpvrotate(vel1, cpvforangle(M_PI/3));
        auto vel3 = cpvrotate(vel1, cpvforangle(-M_PI/3));
        gameScene.addObject(std::make_unique<TestBossProjectile>(gameScene, pos + cpVect{0, 80}, vel1));
        gameScene.addObject(std::make_unique<TestBossProjectile>(gameScene, pos + cpVect{0, 80}, vel2));
        gameScene.addObject(std::make_unique<TestBossProjectile>(gameScene, pos + cpVect{0, 80}, vel3));
    }
}

void TestBoss::die()
{
    EnemyCommon::die();
    
    auto grav = gameScene.getGameSpace().getGravity();
    auto displayGravity = sf::Vector2f(grav.x, grav.y);
    
    auto explosion = std::make_unique<TextureExplosion>(gameScene, mainSprite.getTexture(), ExplosionDuration,
        sf::FloatRect(-80, -32, 160, 16), displayGravity, TextureExplosion::Density, 8, 8, 160);
    explosion->setPosition(getDisplayPosition());
    gameScene.addObject(std::move(explosion));
    
    gameScene.addObject(std::make_unique<TimedLevelWarper>(gameScene, "level2.lvl"));
}

size_t TestBoss::playerDamage(Player& player, const std::shared_ptr<cp::Shape> shape) const
{
    return 18;
}

void TestBoss::render(Renderer& renderer)
{
    EnemyCommon::render(renderer);
    
    applyBlinkEffect(mainSprite);
    
    renderer.pushTransform();
    renderer.currentTransform.translate(getDisplayPosition());
    renderer.pushDrawable(mainSprite, {}, 160);
    renderer.popTransform();
}

size_t TestBoss::getMaxHealth() const { return MaxHealth; }

TestBossProjectile::TestBossProjectile(GameScene& scene, cpVect pos, cpVect vel) : Enemy(scene),
    sprite(scene.getResourceManager().load<sf::Texture>("test-boss-projectile.png"))
{
    setupPhysics();
    collisionBody->setPosition(pos);
    collisionBody->setVelocity(vel);
}

void TestBossProjectile::setupPhysics()
{
    using namespace cp;
    collisionBody = std::make_shared<Body>(0.0, 0.0);
 
    collisionShape = std::make_shared<CircleShape>(collisionBody, 16);
    collisionShape->setDensity(1);
    collisionShape->setCollisionType(Enemy::CollisionType);
    collisionShape->setElasticity(1.0);
    collisionShape->setFriction(0.0);
    gameScene.getGameSpace().add(collisionShape);
    
    gameScene.getGameSpace().add(collisionBody);
    
    Enemy::setupPhysics();
}

TestBossProjectile::~TestBossProjectile()
{
    gameScene.getGameSpace().remove(collisionShape);
    gameScene.getGameSpace().remove(collisionBody);
}

bool TestBossProjectile::onCollisionAttack(Player& player, std::shared_ptr<cp::Shape> shape)
{
    player.damage(10);
    remove();
    return false;
}

void TestBossProjectile::update(std::chrono::steady_clock::time_point curTime)
{
    collisionBody->eachArbiter([=] (cp::Arbiter arbiter)
    {
        if (cpShapeGetCollisionType(arbiter.getShapeA()) == Room::CollisionType ||
            cpShapeGetCollisionType(arbiter.getShapeB()) == Room::CollisionType)
            remove();
    });
}

void TestBossProjectile::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(getDisplayPosition());
    renderer.pushDrawable(sprite, {}, 156);
    renderer.popTransform();
}

void TimedLevelWarper::update(std::chrono::steady_clock::time_point curTime)
{
    if (isNull(initTime)) initTime = curTime;
    
    if (curTime > initTime + std::chrono::seconds(5))
    {
        gameScene.requestLevelLoad(levelName);
        remove();
    }
}
