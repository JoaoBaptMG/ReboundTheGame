#include "InteractionPopup.hpp"

#include "scene/GameScene.hpp"
#include "language/LocalizationManager.hpp"
#include "language/LangID.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "language/convenienceConfigText.hpp"

#include <chronoUtils.hpp>

auto FadeInterval = 12_frames;

InteractionPopup::InteractionPopup(GameScene& scene)
    : GameObject(scene), collapsing(false),
      popupBox(scene.getResourceManager().load<sf::Texture>("popup-background.png")),
      popupLabel(scene.getResourceManager().load<FontHandler>(scene.getLocalizationManager().getFontName()))
{
    popupBox.setOpacity(0);

    auto str = gameScene.getInputSpecifierMap().find("dashbtn")->second;
    popupLabel.setString(str.substr(4, str.size()-8));
    popupLabel.setFontSize(24);
    popupLabel.setDefaultColor(sf::Color(255, 255, 255, 0));
    popupLabel.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    popupLabel.setVerticalAnchor(TextDrawable::VertAnchor::Center);
    configTextDrawable(popupLabel, scene.getLocalizationManager());
    popupLabel.buildGeometry();

    auto bounds = popupLabel.getLocalBounds();

    popupBox.setCenterRect(sf::FloatRect(8, 8, 8, 8));
    popupBox.setDestinationRect(sf::FloatRect(0, 0, bounds.width + 8, bounds.height + 8));
    popupBox.setAnchorPoint(sf::Vector2f(bounds.width/2 + 4, bounds.height/2 + 4));
}

void InteractionPopup::update(FrameTime curTime)
{
    this->curTime = curTime;
    if (isNull(destTime)) destTime = curTime + FadeInterval;

    if (curTime <= destTime)
    {
        float factor = toSeconds<float>(destTime - curTime) / toSeconds<float>(FadeInterval);
        if (!collapsing) factor = 1 - factor;

        popupBox.setOpacity(factor);

        for (auto& v : popupLabel.getAllVertices())
            v.color.a = (sf::Uint8)std::max<int>(255 * factor, 0);
        for (auto& v : popupLabel.getAllVertices(true))
            v.color.a = (sf::Uint8)std::max<int>(255 * factor, 0);
    }
    else if (collapsing) remove();
}

void InteractionPopup::collapse()
{
    collapsing = true;
    destTime = curTime + FadeInterval;
}

bool InteractionPopup::notifyScreenTransition(cpVect displacement)
{
    position += sf::Vector2f(roundf(displacement.x), roundf(displacement.y));
    return true;
}

void InteractionPopup::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(position - sf::Vector2f(0, popupBox.getAnchorPoint().y));
    renderer.pushDrawable(popupBox, {}, 3500);
    renderer.pushDrawable(popupLabel, {}, 3500);
    renderer.popTransform();
}
