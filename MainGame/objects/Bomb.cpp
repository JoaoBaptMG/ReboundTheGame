#include "Bomb.hpp"

#include "defaults.hpp"
#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"

using namespace std::literals::chrono_literals;

constexpr auto DetonationTime = 48 * UpdateFrequency;

Bomb::Bomb(GameScene& scene, cpVect pos, std::chrono::steady_clock::time_point initialTime)
    : GameObject(scene), position(pos), detonationTime(initialTime + DetonationTime),
    sprite(scene.getResourceManager().load<sf::Texture>("bomb.png"))
{
    
}

void Bomb::update(std::chrono::steady_clock::time_point curTime)
{
    if (curTime > detonationTime)
    {
        detonate();
        remove();
    }
}

void Bomb::detonate()
{
    gameScene.getGameSpace().pointQuery(position, 48, CP_SHAPE_FILTER_ALL,
    [=](cpShape* shape, cpVect point, cpFloat distance, cpVect gradient)
    {
        if (distance <= 48 && cpShapeGetCollisionType(shape) == Bombable)
            (*(std::function<void(Bomb*)>*)cpShapeGetUserData(shape))(this);
    });
}

void Bomb::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(getDisplayPosition());
    renderer.pushDrawable(sprite, {}, 18);
    renderer.popTransform();
}
