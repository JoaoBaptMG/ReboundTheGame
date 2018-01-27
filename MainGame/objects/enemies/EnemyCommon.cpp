#include "EnemyCommon.hpp"

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include <chronoUtils.hpp>

#include "objects/Player.hpp"
#include "objects/Bomb.hpp"

using namespace enemies;

constexpr auto BlinkDuration = 12 * UpdateFrequency;

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

void EnemyCommon::update(std::chrono::steady_clock::time_point curTime)
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
