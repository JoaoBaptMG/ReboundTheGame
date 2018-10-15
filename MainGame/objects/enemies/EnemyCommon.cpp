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

#include "EnemyCommon.hpp"

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include <chronoUtils.hpp>

#include "objects/Player.hpp"
#include "objects/Bomb.hpp"

using namespace enemies;

constexpr auto BlinkDuration = 12_frames;

EnemyCommon::EnemyCommon(GameScene& gameScene) : Enemy(gameScene), touchDamage(0), curTime(), blinkTime()
{
}

EnemyCommon::~EnemyCommon()
{
    
}


bool EnemyCommon::onCollisionHit(Player& player, std::shared_ptr<cp::Shape> shape)
{
    damage(touchDamage);
    return true;
}

bool EnemyCommon::onCollisionAttack(Player& player, std::shared_ptr<cp::Shape> shape)
{
    player.damage(playerDamage(player, shape));
    return true;
}

void EnemyCommon::damage(size_t amount)
{
    if (!isNull(blinkTime)) return;
    
    if (health <= amount)
    {
        health = 0;
        die();
    }
    else health -= amount;
    
    blinkTime = curTime;
}

void EnemyCommon::die()
{
    remove();
}

void EnemyCommon::update(FrameTime curTime)
{
    this->curTime = curTime;
    
    if (curTime > blinkTime + BlinkDuration)
        reset(blinkTime);
}

void EnemyCommon::applyBlinkEffect(Sprite& sprite)
{
    if (isNull(blinkTime)) return;
    
    auto color = sf::Color::White;
    float factor = toSeconds<float>(curTime - blinkTime) / toSeconds<float>(BlinkDuration);
    color.a = 80 * (1 - factor);
    
    sprite.setFlashColor(color);
}

void EnemyCommon::addDashShape(std::shared_ptr<cp::Shape> shape, InteractionHandler& handler)
{
    handler = [=](uint32_t ty, void* ptr)
    {
        if (ty == Player::DashInteractionType) damage(dashDamage);
    };
    
    shape->setCollisionType(Interactable);
    shape->setUserData((void*)&handler);
}

void EnemyCommon::addBombShape(std::shared_ptr<cp::Shape> shape, InteractionHandler& handler)
{
    handler = [=](uint32_t ty, void* ptr)
    {
        if (ty == Bomb::InteractionType) damage(bombDamage);
    };
    
    shape->setSensor(true);
    shape->setCollisionType(Interactable);
    shape->setUserData((void*)&handler);
}

void EnemyCommon::render(Renderer& renderer)
{
    
}
