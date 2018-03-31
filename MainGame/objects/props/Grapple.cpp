#include "Grapple.hpp"

#include <SFML/Graphics.hpp>
#include <chipmunk/chipmunk.h>

#include "objects/GameObjectFactory.hpp"

#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "scene/GameScene.hpp"
#include "objects/Player.hpp"
#include <chronoUtils.hpp>
#include <vector_math.hpp>
#include <cpVectOperations.h>

using namespace props;

constexpr float ExciteRadius = 192;
constexpr float ExciteForce = 512;

constexpr auto WobblePeriod = 60 * UpdatePeriod;
constexpr auto GrappleFade = 30 * UpdatePeriod;

Grapple::Grapple(GameScene& gameScene) : GameObject(gameScene), isExcited(false), lastFade(0),
    sprite(gameScene.getResourceManager().load<sf::Texture>("grapple.png")),
    grappleSprite(gameScene.getResourceManager().load<sf::Texture>("grapple-beam.png"))
{
    grappleSprite.setOpacity(0);
}

Grapple::~Grapple()
{
    if (isExcited)
    {
        auto player = gameScene.getObjectByName<Player>("player");
        if (player) player->setGrappling(false);
    }
}

bool Grapple::configure(const Grapple::ConfigStruct &config)
{
    pos = cpVect{(cpFloat)config.position.x, (cpFloat)config.position.y};

    return true;
}

void Grapple::update(std::chrono::steady_clock::time_point curTime)
{
    this->curTime = curTime;
    
    if (initialTime == decltype(initialTime)())
        initialTime = curTime;

    auto player = gameScene.getObjectByName<Player>("player");
    if (player)
    {
        bool newIsExcited = player->canGrapple() &&
            cpvlengthsq(pos - player->getPosition()) <= ExciteRadius*ExciteRadius;
        
        if (newIsExcited != isExcited)
        {
            exciteTime = curTime;
            player->setGrappling(newIsExcited);
        }
        isExcited = newIsExcited;

        if (isExcited)
        {
            auto body = player->getBody();
            auto t = cpvlength(pos - player->getPosition())/ExciteRadius;
            auto param = ExciteForce;
            auto n = cpvnormalize(pos - player->getPosition());
            auto vdot = cpvdot(n, body->getVelocity());
            auto dt = toSeconds<cpFloat>(UpdatePeriod);

            body->applyForceAtLocalPoint(n * param * body->getMass(), cpvzero);
        }
    }
    else
    {
        if (isExcited) exciteTime = curTime;
        isExcited = false;
    }
}

bool Grapple::notifyScreenTransition(cpVect displacement)
{
    pos += displacement;
    return true;
}

void Grapple::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(getDisplayPosition());

    renderer.pushTransform();

    auto phases = toSeconds<float>(curTime - initialTime) / toSeconds<float>(WobblePeriod);
    auto scaleFactor = 0.875f + 0.125f * cosf(2 * M_PI * phases);
    renderer.currentTransform.scale(scaleFactor, scaleFactor);
    renderer.pushDrawable(sprite, {}, 20);
    renderer.popTransform();

    auto fade = std::min(toSeconds<float>(curTime - exciteTime) / toSeconds<float>(GrappleFade), 1.0f);
    if (!isExcited) fade = std::min(1.0f - fade, lastFade);
    else fade = std::max(fade, lastFade);
    lastFade = fade;
    grappleSprite.setOpacity(fade);
    
    if (fade != 0.0)
    {
        auto player = gameScene.getObjectByName<Player>("player");
        if (player)
        {
            auto vec = player->getDisplayPosition() - getDisplayPosition();
            renderer.pushTransform();
            renderer.currentTransform.translate(vec/2.0f);
            renderer.currentTransform.rotate(radiansToDegrees(angle(vec)));
            renderer.currentTransform.scale(length(vec)/grappleSprite.getTexture()->getSize().x, 1.0f);
            renderer.pushDrawable(grappleSprite, {}, 15);
            renderer.popTransform();
        }
    }

    renderer.popTransform();
}

REGISTER_GAME_OBJECT(props::Grapple);
