#include "Floater.hpp"

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "utility/chronoUtils.hpp"

using namespace enemies;

constexpr float Period = 6;
constexpr float Amplitude = 18;

Floater::Floater(GameScene& gameScene) : GameObject(gameScene),
    sprite(gameScene.getResourceManager().load<sf::Texture>("floater.png"))
{
}

bool Floater::configure(const Floater::ConfigStruct& config)
{
    position = config.position;
    return true;
}

void Floater::update(std::chrono::steady_clock::time_point curTime)
{
    if (initialTime == decltype(initialTime)())
        initialTime = curTime;

    float duration = toSeconds<float>(curTime - initialTime);
    yDisplacement = -Amplitude * sinf(6.28318530718 * duration / Period);
}

void Floater::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(position);
    renderer.currentTransform.translate(0, yDisplacement);
    renderer.pushDrawable(sprite, {}, 25);
    renderer.popTransform();
}
