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


#include "UIFileSelectButton.hpp"

#include "gameplay/SavedGame.hpp"
#include "rendering/Renderer.hpp"
#include "resources/ResourceManager.hpp"
#include "language/LocalizationManager.hpp"
#include "language/LangID.hpp"
#include "language/convenienceConfigText.hpp"
#include "ui/UIButtonCommons.hpp"
#include "drawables/SegmentedSprite.hpp"
#include "data/LevelData.hpp"

constexpr float TextSize = 32;
constexpr float ButtonSize = 576;
constexpr float ButtonBorder = 4;
constexpr float ButtonSpace = 8;

UIFileSelectButton::UIFileSelectButton(const SavedGame& sg, Services& services, size_t index)
    : UIButton(services.inputManager), rtl(services.localizationManager.isRTL()),
    goldenTokenSprite(services.resourceManager.load<sf::Texture>("golden-token.png")),
    picketSprite(services.resourceManager.load<sf::Texture>("icon-picket.png")),
    fileName(loadDefaultFont(services)),
    goldenTokenAmount(loadDefaultFont(services)),
    picketAmount(loadDefaultFont(services))
{
    sf::FloatRect centerRect(4, 4, 4, 4);
    sf::FloatRect destRect(0, 0, ButtonSize, 128);
    sf::Vector2f destCenter(destRect.width/2, destRect.height/2);
    
    auto normalSprite = std::make_unique<SegmentedSprite>(
        services.resourceManager.load<sf::Texture>("ui-file-button-frame.png"));
    normalSprite->setCenterRect(centerRect);
    normalSprite->setDestinationRect(destRect);
    normalSprite->setAnchorPoint(destCenter);
    
    auto activeSprite = std::make_unique<SegmentedSprite>(
        services.resourceManager.load<sf::Texture>("ui-file-button-frame-active.png"));
    activeSprite->setCenterRect(centerRect);
    activeSprite->setDestinationRect(destRect);
    activeSprite->setAnchorPoint(destCenter);
    
    auto pressedSprite = std::make_unique<SegmentedSprite>(
        services.resourceManager.load<sf::Texture>("ui-file-button-frame-pressed.png"));
    pressedSprite->setCenterRect(centerRect);
    pressedSprite->setDestinationRect(destRect);
    pressedSprite->setAnchorPoint(destCenter);
    
    setNormalSprite(std::move(normalSprite));
    setActiveSprite(std::move(activeSprite));
    setPressedSprite(std::move(pressedSprite));
    autoComputeBounds();
    
    size_t k = 0;
    for (auto& sprite : powerupSprites)
    {
        sprite.setTexture(services.resourceManager.load<sf::Texture>("powerup" + std::to_string(k+1) + ".png"));
        bool is = sg.getAbilityLevel() > k;
        sprite.setBlendColor(sf::Color(is ? 255 : 0, is ? 255 : 0, is ? 255 : 0, 255));
        k++;
    }

    auto& lm = services.localizationManager;
    auto indexStr = lm.getFormattedString("file-select-index", {}, { { "i", index+1 } }, {});
    fileName.setFontHandler(loadDefaultFont(services));
    fileName.setString(indexStr + ' ' + getLevelNameForNumber(lm, sg.getCurLevel()));
    fileName.setFontSize(TextSize);
    fileName.setDefaultColor(sf::Color::White);
    fileName.setOutlineThickness(1);
    fileName.setDefaultOutlineColor(sf::Color::Black);
    fileName.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    fileName.setVerticalAnchor(TextDrawable::VertAnchor::Center);
    fileName.setWordWrappingWidth(ButtonSize - 2 * (ButtonBorder + ButtonSpace));
    fileName.setWordAlignment(TextDrawable::Alignment::Direct);
    configTextDrawable(fileName, lm);
    fileName.buildGeometry();

    auto gtStr = lm.getFormattedString("file-select-golden-token-count", {}, { { "n", sg.getGoldenTokenCount() } }, {});
    goldenTokenAmount.setFontHandler(loadDefaultFont(services));
    goldenTokenAmount.setString(gtStr);
    goldenTokenAmount.setFontSize(TextSize);
    goldenTokenAmount.setDefaultColor(sf::Color::White);
    goldenTokenAmount.setOutlineThickness(1);
    goldenTokenAmount.setDefaultOutlineColor(sf::Color::Black);
    goldenTokenAmount.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    goldenTokenAmount.setVerticalAnchor(TextDrawable::VertAnchor::Center);
    configTextDrawable(goldenTokenAmount, lm);
    goldenTokenAmount.buildGeometry();

    auto pStr = lm.getFormattedString("file-select-picket-count", {}, { { "n", sg.getPicketCount() } }, {});
    picketAmount.setFontHandler(loadDefaultFont(services));
    picketAmount.setString(pStr);
    picketAmount.setFontSize(TextSize);
    picketAmount.setDefaultColor(sf::Color::White);
    picketAmount.setOutlineThickness(1);
    picketAmount.setDefaultOutlineColor(sf::Color::Black);
    picketAmount.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    picketAmount.setVerticalAnchor(TextDrawable::VertAnchor::Center);
    configTextDrawable(picketAmount, lm);
    picketAmount.buildGeometry();
    
    setDepth(10);
}

sf::FloatRect UIFileSelectButton::getBounds() const
{
    return sf::FloatRect(-ButtonSize/2, 0, ButtonSize, 128);
}

void UIFileSelectButton::render(Renderer& renderer)
{
    UIButton::render(renderer);
    
    renderer.pushTransform();
    renderer.currentTransform.translate(getPosition());
    
    renderer.pushTransform();
    renderer.currentTransform.translate(0, -28);
    renderer.pushDrawable(fileName, {}, getDepth()+4);
    renderer.popTransform();
    
    renderer.pushTransform();
    renderer.currentTransform.translate(rtl ? ButtonSize/2 - ButtonBorder - 56 :
        -ButtonSize/2 + ButtonBorder + ButtonSpace, 4);
        
    for (auto& sprite : powerupSprites)
    {
        renderer.pushDrawable(sprite, {}, getDepth()+4);
        renderer.currentTransform.translate(rtl ? -56 : 56, 0);
    }
    
    renderer.popTransform();
    
    renderer.pushTransform();
    auto bounds = goldenTokenAmount.getLocalBounds();
    float disp = -ButtonSize/2 + ButtonBorder + ButtonSpace + bounds.width/2;
    renderer.currentTransform.translate(rtl ? disp : -disp, -28);
    renderer.pushDrawable(goldenTokenAmount, {}, getDepth()+4);
    disp = bounds.width/2 + ButtonSpace + 24;
    renderer.currentTransform.translate(rtl ? disp : -disp, 0);
    renderer.pushDrawable(goldenTokenSprite, {}, getDepth()+4);
    bounds = picketAmount.getLocalBounds();
    disp = bounds.width/2 + ButtonSpace + 36;
    renderer.currentTransform.translate(rtl ? disp : -disp, 0);
    renderer.pushDrawable(picketAmount, {}, getDepth()+4);
    disp = bounds.width/2 + ButtonSpace + 16;
    renderer.currentTransform.translate(rtl ? disp : -disp, 4);
    renderer.pushDrawable(picketSprite, {}, getDepth()+4);

    renderer.popTransform();
    
    renderer.popTransform();
}
