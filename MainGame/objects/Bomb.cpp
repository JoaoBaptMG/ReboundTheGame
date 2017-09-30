#include "Bomb.hpp"

#include "defaults.hpp"
#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "particles/ParticleBatch.hpp"

#include "objects/Player.hpp"

using namespace std::literals::chrono_literals;

constexpr auto DetonationTime = 48 * UpdateFrequency;

Bomb::Bomb(GameScene& scene, cpVect pos, Player* player, std::chrono::steady_clock::time_point initialTime)
    : GameObject(scene), position(pos), player(player), detonationTime(initialTime + DetonationTime),
    sprite(scene.getResourceManager().load<sf::Texture>("bomb.png"))
{
    
}

void Bomb::update(std::chrono::steady_clock::time_point curTime)
{
    if (curTime > detonationTime)
    {
        player->numBombs++;
        detonate();
        remove();
    }
}

void Bomb::detonate()
{
    auto batch = std::make_unique<ParticleBatch>(gameScene, "player-particles.pe", "bomb");
    batch->setPosition(getDisplayPosition());
    gameScene.addObject(std::move(batch));
    
    gameScene.getGameSpace().pointQuery(position, 48, CP_SHAPE_FILTER_ALL,
    [=](cpShape* shape, cpVect point, cpFloat distance, cpVect gradient)
    {
        if (distance <= 48 && cpShapeGetCollisionType(shape) == Interactable)
            (*(GameObject::InteractionHandler*)cpShapeGetUserData(shape))(InteractionType, (void*)this);
    });
}

void Bomb::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(getDisplayPosition());
    renderer.pushDrawable(sprite, {}, 14);
    renderer.popTransform();
}
