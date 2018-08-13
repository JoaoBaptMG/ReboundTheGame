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


#include "UIButtonCommons.hpp"

#include "resources/ResourceManager.hpp"
#include "resources/FontHandler.hpp"
#include "language/LocalizationManager.hpp"
#include "language/convenienceConfigText.hpp"
#include "drawables/Sprite.hpp"
#include "drawables/SegmentedSprite.hpp"
#include "rendering/Texture.hpp"

#include "audio/AudioManager.hpp"
#include "audio/Sound.hpp"

void createCommonTextualButton(UIButton& button, Services& services,
    std::string activeResourceName, std::string pressedResourceName, util::rect centerRect,
    util::rect destRect, LangID captionString, size_t captionSize, glm::u8vec4 textColor,
    float outlineThickness, glm::u8vec4 outlineColor, glm::vec2 captionDisplacement,
    TextDrawable::Alignment wordAlignment)
{
    glm::vec2 destCenter(destRect.width/2, destRect.height/2);
    
    auto activeSprite = std::make_unique<SegmentedSprite>(services.resourceManager.load<Texture>(activeResourceName));
    activeSprite->setCenterRect(centerRect);
    activeSprite->setDestinationRect(destRect);
    activeSprite->setAnchorPoint(destCenter);
    
    auto pressedSprite = std::make_unique<SegmentedSprite>(services.resourceManager.load<Texture>(pressedResourceName));
    pressedSprite->setCenterRect(centerRect);
    pressedSprite->setDestinationRect(destRect);
    pressedSprite->setAnchorPoint(destCenter);
    
    auto caption = std::make_unique<TextDrawable>(loadDefaultFont(services));
    if (!captionString.empty()) caption->setString(services.localizationManager.getString(captionString));
    caption->setFontSize(captionSize);
    caption->setDefaultColor(textColor);
    caption->setOutlineThickness(outlineThickness);
    caption->setDefaultOutlineColor(outlineColor);
    caption->setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    caption->setVerticalAnchor(TextDrawable::VertAnchor::Center);
    caption->setWordWrappingWidth(destRect.width - 2 * captionDisplacement.x);
    caption->setWordAlignment(wordAlignment);
    configTextDrawable(*caption, services.localizationManager);
    caption->buildGeometry();
    
    button.setActiveSprite(std::move(activeSprite));
    button.setPressedSprite(std::move(pressedSprite));
    button.setCaption(std::move(caption));
    button.setCaptionDisplacement(captionDisplacement);
    button.autoComputeBounds();

    button.setOverAction([&] { playCursor(services); });
}

void createCommonGraphicsButton(UIButton& button, ResourceManager& rm, std::string normalResourceName,
    std::string activeResourceName, std::string pressedResourceName)
{
    
}

void playConfirm(Services& services)
{
    services.audioManager.playSound(services.resourceManager.load<Sound>("ui-confirm.wav")); 
}

void playCursor(Services& services)
{
    services.audioManager.playSound(services.resourceManager.load<Sound>("ui-cursor.wav"));
}
