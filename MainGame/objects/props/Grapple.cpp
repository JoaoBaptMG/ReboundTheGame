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

#include "Grapple.hpp"


#include <chipmunk/chipmunk.h>

#include "objects/GameObjectFactory.hpp"

#include "rendering/Renderer.hpp"
#include "rendering/Texture.hpp"
#include "resources/ResourceManager.hpp"
#include "scene/GameScene.hpp"
#include "objects/Player.hpp"
#include <chronoUtils.hpp>
#include <vector_math.hpp>
#include <cpVectOperations.h>

using namespace props;

constexpr float ExciteRadius = 192;
constexpr float ExciteForce = 512;

constexpr auto WobblePeriod = 60_frames;
constexpr auto GrappleFade = 30_frames;

Grapple::Grapple(GameScene& gameScene) : GameObject(gameScene), isExcited(false), lastFade(0),
    sprite(gameScene.getResourceManager().load<Texture>("grapple.png")),
    grappleSprite(gameScene.getResourceManager().load<Texture>("grapple-beam.png"))
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

void Grapple::update(FrameTime curTime)
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
    renderer.currentTransform *= util::translate(getDisplayPosition());

    renderer.pushTransform();

    auto phases = toSeconds<float>(curTime - initialTime) / toSeconds<float>(WobblePeriod);
    auto scaleFactor = 0.875f + 0.125f * cosf(2 * M_PI * phases);
    renderer.currentTransform *= util::scale(scaleFactor);
    renderer.pushDrawable(sprite, 20);
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
            renderer.currentTransform *= util::translate(vec/2.0f);
            renderer.currentTransform *= util::rotate(angle(vec));
            renderer.currentTransform *= util::scale(length(vec)/grappleSprite.getTexture()->getSize().x, 1.0f);
            renderer.pushDrawable(grappleSprite, 15);
            renderer.popTransform();
        }
    }

    renderer.popTransform();
}

REGISTER_GAME_OBJECT(props::Grapple);
