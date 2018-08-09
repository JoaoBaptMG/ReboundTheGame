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


#include "InteractionPopup.hpp"

#include "scene/GameScene.hpp"
#include "language/LocalizationManager.hpp"
#include "language/LangID.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "language/convenienceConfigText.hpp"

#include <chronoUtils.hpp>
#include <transforms.hpp>

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
    popupLabel.setDefaultColor(glm::u8vec4(255, 255, 255, 0));
    popupLabel.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    popupLabel.setVerticalAnchor(TextDrawable::VertAnchor::Center);
    configTextDrawable(popupLabel, scene.getLocalizationManager());
    popupLabel.buildGeometry();

    auto bounds = popupLabel.getLocalBounds();

    popupBox.setCenterRect(util::rect(8, 8, 8, 8));
    popupBox.setDestinationRect(util::rect(0, 0, bounds.width + 8, bounds.height + 8));
    popupBox.setAnchorPoint(glm::vec2(bounds.width/2 + 4, bounds.height/2 + 4));
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
    position += glm::vec2(roundf(displacement.x), roundf(displacement.y));
    return true;
}

void InteractionPopup::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform *= util::translate(position - glm::vec2(0, popupBox.getAnchorPoint().y));
    renderer.pushDrawable(popupBox, {}, 3500);
    renderer.pushDrawable(popupLabel, {}, 3500);
    renderer.popTransform();
}
