#include "Floater.hpp"

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"

using namespace enemies;

Floater::Floater(GameScene& gameScene) : GameObject(gameScene)
{
    sprite.setTexture(*gameScene.getResourceManager().load<sf::Texture>("floater.png"));
    sprite.setOrigin(24, 48);
}

bool Floater::configure(const Floater::ConfigStruct& config)
{
    position = config.position;
    return true;
}

void Floater::update(std::chrono::steady_clock::time_point curTime)
{
}

void Floater::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(position);
    renderer.pushDrawable(sprite, {}, 25);
    renderer.popTransform();
}
