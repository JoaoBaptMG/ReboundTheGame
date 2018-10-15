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

#include "Bomb.hpp"

#include "defaults.hpp"
#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Texture.hpp"
#include "resources/ResourceManager.hpp"
#include "particles/ParticleBatch.hpp"

#include <transforms.hpp>

#include "objects/Player.hpp"

using namespace std::literals::chrono_literals;

constexpr auto DetonationTime = 48_frames;
constexpr float SinePhase = 4;
constexpr float SinePower = 2;

Bomb::Bomb(GameScene& scene, cpVect pos, FrameTime initialTime)
    : GameObject(scene), position(pos), detonationTime(initialTime + DetonationTime),
    sprite(scene.getResourceManager().load<Texture>("bomb.png"))
{
    
}

Bomb::~Bomb()
{
    auto player = gameScene.getObjectByName<Player>("player");
    if (player) player->numBombs++;
}

void Bomb::update(FrameTime curTime)
{
    this->curTime = curTime;
    
    if (curTime > detonationTime)
    {
        detonate();
        remove();
    }
}

void Bomb::detonate()
{
    auto batch = std::make_unique<ParticleBatch>(gameScene, "player-particles.pe", "bomb");
    batch->setPosition(getDisplayPosition());
    gameScene.addObject(std::move(batch));

    gameScene.playSound("bomb-detonate.wav");

    gameScene.getGameSpace().pointQuery(position, 48, CP_SHAPE_FILTER_ALL,
    [=](std::shared_ptr<cp::Shape> shape, cpVect point, cpFloat distance, cpVect gradient)
    {
        if (distance <= 48 && shape->getCollisionType() == Interactable)
            (*(GameObject::InteractionHandler*)shape->getUserData())(InteractionType, (void*)this);
    });
}

bool Bomb::notifyScreenTransition(cpVect displacement)
{
    position = position + displacement;
    return true;
}

void Bomb::render(Renderer& renderer)
{
    float factor = 1.0f - toSeconds<float>(detonationTime - curTime) / toSeconds<float>(DetonationTime);
    sprite.setOpacity(0.75f + 0.25f * cosf(2 * M_PI * SinePhase * powf(factor, SinePower)));
    
    renderer.pushTransform();
	renderer.currentTransform *= util::translate(getDisplayPosition());
    renderer.pushDrawable(sprite, 14);
    renderer.popTransform();
}
