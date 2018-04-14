#include "Boss.hpp"

#include "scene/GameScene.hpp"
#include <chronoUtils.hpp>
#include "defaults.hpp"

#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "language/LocalizationManager.hpp"
#include "language/convenienceConfigText.hpp"

using namespace enemies;

Boss::Boss(GameScene& scene, LangID presenterID) : EnemyCommon(scene)
{
    scene.setCurrentBoss(this);
    scene.addObject(std::make_unique<BossCaption>(scene, presenterID));
}

Boss::~Boss()
{
    gameScene.setCurrentBoss(nullBossMeter());
}

constexpr auto CaptionDuration = 90_frames;
constexpr auto CaptionFade = 15_frames;

BossCaption::BossCaption(GameScene& scene, LangID captionID) : GameObject(scene),
    text(scene.getResourceManager().load<FontHandler>(scene.getLocalizationManager().getFontName()))
{
    text.setString(scene.getLocalizationManager().getString(captionID));
    text.setFontSize(48);
    text.setDefaultColor(sf::Color::White);
    text.setOutlineThickness(2);
    text.setDefaultOutlineColor(sf::Color::Black);
    text.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    text.setVerticalAnchor(TextDrawable::VertAnchor::Center);
    configTextDrawable(text, scene.getLocalizationManager());
    text.buildGeometry();
}

void BossCaption::update(FrameTime curTime)
{
    this->curTime = curTime;
    if (isNull(initTime)) initTime = curTime;
    
    if (curTime > initTime + CaptionDuration + 2 * CaptionFade) remove();
}

void BossCaption::render(Renderer& renderer)
{
    float factor = 1;
    if (curTime < initTime + CaptionFade)
        factor = toSeconds<float>(curTime - initTime) / toSeconds<float>(CaptionFade);
    else if (curTime > initTime + CaptionFade + CaptionDuration)
        factor = 1 - toSeconds<float>(curTime - initTime - CaptionFade - CaptionDuration) /
            toSeconds<float>(CaptionFade);
    
    for (auto& vtx : text.getAllVertices()) vtx.color.a = 255 * factor;
    for (auto& vtx : text.getAllVertices(true)) vtx.color.a = 255 * factor;
    
    auto transform = renderer.currentTransform;
    renderer.popTransform();
    renderer.pushTransform();
    renderer.currentTransform.translate(ScreenWidth/2, ScreenHeight/2);
    renderer.pushDrawable(text, {}, 120000);
    renderer.popTransform();
    renderer.pushTransform();
    renderer.currentTransform = transform;
}
