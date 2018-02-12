#include "GoldenToken.hpp"

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include <chronoUtils.hpp>

#include <cppmunk/CircleShape.h>

using namespace collectibles;

constexpr cpFloat Amplitude = 8;
constexpr cpFloat Period = 2.5;

GoldenToken::GoldenToken(GameScene& scene) : Collectible(scene), 
    sprite(gameScene.getResourceManager().load<sf::Texture>("golden-token.png"))
{
    setupPhysics();
    
    auto batch = std::make_unique<ParticleBatch>(gameScene, "golden-token-particles.pe", "golden-token");
    tokenBatch = batch.get();
    gameScene.addObject(std::move(batch));
}

bool GoldenToken::configure(const GoldenToken::ConfigStruct& config)
{
    auto pos = cpVect{ (float)config.position.x, (float)config.position.y };
    collisionBody->setPosition(pos);
    baseY = pos.y;
    
    tokenId = config.tokenId;
    
    return !gameScene.getSavedGame().getGoldenToken(tokenId);
}

void GoldenToken::setupPhysics()
{
    using namespace cp;

    collisionBody = std::make_shared<Body>(Body::Kinematic);
    collisionShape = std::make_shared<CircleShape>(collisionBody, 24, cpVect{ 0.0, 0.0 });

    collisionShape->setCollisionType(Collectible::CollisionType);
    collisionShape->setSensor(true);

    gameScene.getGameSpace().add(collisionBody);
    gameScene.getGameSpace().add(collisionShape);
    
    Collectible::setupPhysics();
}

GoldenToken::~GoldenToken()
{
    gameScene.getGameSpace().remove(collisionShape);
    gameScene.getGameSpace().remove(collisionBody);
    
    tokenBatch->abort();
}

void GoldenToken::onCollect(Player& player)
{
    gameScene.getSavedGame().setGoldenToken(tokenId, true);
    player.upgradeHealth();
    remove();
}

void GoldenToken::update(std::chrono::steady_clock::time_point curTime)
{
    if (initialTime == decltype(initialTime)())
        initialTime = curTime;
        
    cpFloat factor = sin(2 * M_PI * toSeconds<cpFloat>(curTime - initialTime) / Period);
    auto pos = collisionBody->getPosition();
    pos.y = baseY - Amplitude * factor;
    collisionBody->setPosition(pos);
    
    tokenBatch->setPosition(sf::Vector2f(pos.x, pos.y));
}

bool GoldenToken::notifyScreenTransition(cpVect displacement)
{
    baseY += displacement.y;
    return Collectible::notifyScreenTransition(displacement);
}

void GoldenToken::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(getDisplayPosition());
    renderer.pushDrawable(sprite, {}, 25);
    renderer.popTransform();
}
