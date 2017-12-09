#include "Parallax.hpp"

#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include "defaults.hpp"

using namespace background;

Parallax::Parallax(GameScene& scene) : GameObject(scene), parallaxFactor(1.0f), internalDisplacement(0, 0),
    plane(sf::FloatRect((float)(ScreenWidth-PlayfieldWidth)/2, (float)(ScreenHeight-PlayfieldHeight)/2,
                        (float)PlayfieldWidth, (float)PlayfieldHeight))
{
}

Parallax::Parallax(GameScene& scene, std::string textureName) : Parallax(scene)
{
    plane.setTexture(scene.getResourceManager().load<sf::Texture>(textureName));
}

bool readFromStream(sf::InputStream& stream, Parallax::ConfigStruct& config)
{
    using namespace util;

    return readFromStream(stream, config.dummy, config.textureName,
        config.parallaxNumerator, config.parallaxDenominator);
}

bool Parallax::configure(const ConfigStruct& config)
{
    auto texture = gameScene.getResourceManager().load<sf::Texture>(config.textureName);
    auto other = gameScene.getObjectByName<Parallax>(getName());
    if (other && other->plane.getTexture() == texture)
    {
        other->transitionState = false;
        return false;
    }

    parallaxFactor = (float)config.parallaxNumerator/(float)config.parallaxDenominator;
    plane.setTexture(texture);

    return true;
}

void Parallax::update(std::chrono::steady_clock::time_point curTime) {}

void Parallax::render(Renderer& renderer)
{
    auto mat = renderer.currentTransform.getMatrix();
    auto trans = sf::Vector2f(mat[12], mat[13]) * (parallaxFactor - 1.0f);
    trans = sf::Vector2f(std::floor(trans.x), std::floor(trans.y));

    renderer.pushTransform();
    renderer.currentTransform.translate(trans);
    renderer.currentTransform.translate(internalDisplacement);
    renderer.pushDrawable(plane, {}, 0);
    renderer.popTransform();
}

bool Parallax::notifyScreenTransition(cpVect displacement)
{
    internalDisplacement += sf::Vector2f(displacement.x, displacement.y) * (1.0f - parallaxFactor);
    return true;
}
