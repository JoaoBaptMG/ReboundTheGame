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


#include "CollectedPauseFrame.hpp"

#include "resources/ResourceManager.hpp"
#include "gameplay/SavedGame.hpp"
#include "rendering/Renderer.hpp"
#include "language/LocalizationManager.hpp"
#include "language/convenienceConfigText.hpp"
#include "drawables/ScissorRectUtils.hpp"
#include "data/LevelData.hpp"
#include "defaults.hpp"
#include <chronoUtils.hpp>

#include "settings/Settings.hpp"
#include "settings/InputSettings.hpp"

const LangID Titles[] =
{
    "pause-collected-powerups",
    "pause-collected-golden-tokens",
    "pause-collected-pickets",
};

constexpr auto TitleSize = 32;
constexpr auto ButtonSize = 24;

constexpr auto RotationPeriod = 72_frames;
constexpr float TravelAmount = 240;

static ScissorRectPush scissorRectPush{};

CollectedPauseFrame::CollectedPauseFrame(const Settings& settings, InputManager& im, ResourceManager& rm,
    LocalizationManager& lm) : localizationManager(lm), showSecretPowerups(false),
    scrollBar(im, rm, 600, 512)
{
    scrollBar.setDepth(3106);
    scrollBar.setPosition(sf::Vector2f((ScreenWidth+PlayfieldWidth)/2 - 8, 64));

    size_t i = 0;
    for (auto& title : titles)
    {
        title.setFontHandler(rm.load<FontHandler>(lm.getFontName()));
        title.setString(lm.getString(Titles[i]));
        title.setFontSize(TitleSize);
        title.setDefaultColor(sf::Color::Yellow);
        title.setOutlineThickness(1);
        title.setDefaultOutlineColor(sf::Color::Black);
        title.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
        title.setVerticalAnchor(TextDrawable::VertAnchor::Center);
        configTextDrawable(title, lm);
        title.buildGeometry();
        i++;
    }

    for (auto& label : powerupLabels)
    {
        label.setFontHandler(rm.load<FontHandler>(lm.getFontName()));
        label.setString(lm.getString("pause-collected-unknown"));
        label.setFontSize(ButtonSize);
        label.setDefaultColor(sf::Color::White);
        label.setOutlineThickness(1);
        label.setDefaultOutlineColor(sf::Color::Black);
        if (lm.isRTL()) label.setHorizontalAnchor(TextDrawable::HorAnchor::Right);
        else label.setHorizontalAnchor(TextDrawable::HorAnchor::Left);
        label.setVerticalAnchor(TextDrawable::VertAnchor::Center);
        configTextDrawable(label, lm);
        label.buildGeometry();
    }

    i = 0;
    for (auto& label : levelLabels)
    {
        label.setFontHandler(rm.load<FontHandler>(lm.getFontName()));
        label.setString(getLevelNameForNumber(lm, i+1));
        label.setFontSize(ButtonSize);
        label.setDefaultColor(sf::Color::White);
        label.setOutlineThickness(1);
        label.setDefaultOutlineColor(sf::Color::Black);
        if (lm.isRTL()) label.setHorizontalAnchor(TextDrawable::HorAnchor::Right);
        else label.setHorizontalAnchor(TextDrawable::HorAnchor::Left);
        label.setVerticalAnchor(TextDrawable::VertAnchor::Center);
        configTextDrawable(label, lm);
        label.buildGeometry();
        i++;
    }

    for (auto& label : picketCount)
    {
        label.setFontHandler(rm.load<FontHandler>(lm.getFontName()));
        label.setString(lm.getFormattedString("pause-collected-picket-count", {}, {{"n",0}}, {}));
        label.setFontSize(ButtonSize);
        label.setDefaultColor(sf::Color::White);
        label.setOutlineThickness(1);
        label.setDefaultOutlineColor(sf::Color::Black);
        if (lm.isRTL()) label.setHorizontalAnchor(TextDrawable::HorAnchor::Left);
        else label.setHorizontalAnchor(TextDrawable::HorAnchor::Right);
        label.setVerticalAnchor(TextDrawable::VertAnchor::Center);
        configTextDrawable(label, lm);
        label.buildGeometry();
    }

    i = 0;
    for (auto plabel : { &totalLabel, &totalPicketCount })
    {
        auto& label = *plabel;
        label.setFontHandler(rm.load<FontHandler>(lm.getFontName()));
        if (i == 0) label.setString(lm.getString("pause-collected-total"));
        else label.setString(lm.getFormattedString("pause-collected-total-picket-count", {}, {{"n",0}}, {}));
        label.setFontSize(TitleSize);
        label.setDefaultColor(sf::Color::White);
        label.setOutlineThickness(1);
        label.setDefaultOutlineColor(sf::Color::Black);
        if (lm.isRTL() ^ (i == 1)) label.setHorizontalAnchor(TextDrawable::HorAnchor::Right);
        else label.setHorizontalAnchor(TextDrawable::HorAnchor::Left);
        label.setVerticalAnchor(TextDrawable::VertAnchor::Center);
        configTextDrawable(label, lm);
        label.buildGeometry();
        i++;
    }
    
    i = 0;
    for (auto& sprite : powerupSprites)
    {
        sprite.setTexture(rm.load<sf::Texture>("powerup" + std::to_string(i+1) + ".png"));
        i++;
    }
    
    for (auto& sprite : goldenTokenSprites)
        sprite.setTexture(rm.load<sf::Texture>("golden-token.png"));

    for (auto& sprite : picketSprites)
    {
        sprite.setTexture(rm.load<sf::Texture>("icon-picket.png"));
        sprite.setAnchorPoint(sf::Vector2f(sprite.getTextureSize()/2u));
        sprite.setBlendColor(sf::Color::Black);
    }

    travel.registerButton(im, settings.inputSettings.keyboardSettings.moveUp,    AxisDirection::Negative, 0);
    travel.registerButton(im, settings.inputSettings.keyboardSettings.moveDown,  AxisDirection::Positive, 0);
    travel.registerAxis(im, settings.inputSettings.joystickSettings.movementAxisY,    0);
    travel.registerAxis(im, settings.inputSettings.joystickSettings.movementAxisYAlt, 1);
}

void CollectedPauseFrame::setSavedGame(const SavedGame& savedGame)
{
    size_t i = 0;
    for (auto& sprite : powerupSprites)
    {
        bool is = i == 10 ? savedGame.getDoubleArmor() :
            i == 11 ? savedGame.getMoveRegen() :
            savedGame.getAbilityLevel() > i;

        sprite.setBlendColor(sf::Color(is ? 255 : 0, is ? 255 : 0, is ? 255 : 0, 255));
        if (is)
        {
            auto name = "powerup" + std::to_string(i+1) + "-name";
            powerupLabels[i].setString(localizationManager.getString(name));
            powerupLabels[i].buildGeometry();
            
            constexpr float MaxWidth = PlayfieldWidth/2 - 72;
            if (powerupLabels[i].getLocalBounds().width > MaxWidth)
            {
                size_t size = powerupLabels[i].getFontSize();
                powerupLabels[i].setFontSize(size * MaxWidth / powerupLabels[i].getLocalBounds().width);
                powerupLabels[i].buildGeometry();
            }
        }
        i++;
    }

    showSecretPowerups = savedGame.getDoubleArmor() || savedGame.getMoveRegen();
    scrollBar.computeSizes(UIScrollBar::Direction::Vertical, getTotalHeight(), 512);
    
    i = 0;
    for (auto& sprite : goldenTokenSprites)
    {
        sprite.setGrayscaleFactor(!savedGame.getGoldenToken(i));
        i++;
    }

    i = 0;
    for (auto& label : picketCount)
    {
        auto n = savedGame.getPicketCountForLevel(i+1);
        auto str = localizationManager.getFormattedString("pause-collected-picket-count", {}, {{"n",n}}, {});
        label.setString(str);
        label.buildGeometry();

        uint8_t b = 255 * n / 100;
        picketSprites[i].setBlendColor(sf::Color(b, b, b));
        i++;
    }

    auto n = savedGame.getPicketCount();
    auto str = localizationManager.getFormattedString("pause-collected-total-picket-count", {}, {{"n",n}}, {});
    totalPicketCount.setString(str);
    totalPicketCount.buildGeometry();
    uint8_t b = 255 * n / 1000;
    picketSprites[10].setBlendColor(sf::Color(b, b, b));
}

void CollectedPauseFrame::setHealthData(size_t curHealth, size_t maxHealth)
{

}

size_t CollectedPauseFrame::getTotalHeight() const
{
    return 864 + (showSecretPowerups ? 56 : 0);
}

void CollectedPauseFrame::update(FrameTime curTime)
{
    if (isNull(initTime)) initTime = curTime;
    this->curTime = curTime;

    auto ofs = scrollBar.getCurrentOffset();
    float dt = toSeconds<float>(UpdatePeriod);
    scrollBar.setCurrentOffset(ofs + TravelAmount * dt * travel.getValue());
}

void CollectedPauseFrame::render(Renderer &renderer)
{
    bool rtl = localizationManager.isRTL();

    auto rect = sf::FloatRect((ScreenWidth - PlayfieldWidth)/2, 0, 576, 576);
    scissorRectPush = ScissorRectPush(renderer.currentTransform.transformRect(rect));
    renderer.pushDrawable(scissorRectPush, {}, 3100);

    renderer.pushTransform();

    renderer.currentTransform.translate(0, -scrollBar.getCurrentOffset());
    renderer.currentTransform.translate(ScreenWidth/2, 28);
    renderer.pushDrawable(titles[0], {}, 3101);
    renderer.currentTransform.translate(-(float)ScreenWidth/2, 24);

    renderer.currentTransform.translate(rtl ? ScreenWidth - 156 : 108, 4);

    constexpr float StandardSpacing = PlayfieldWidth/2 - 8;
    size_t i = 0;
    for (auto& sprite : powerupSprites)
    {
        renderer.pushDrawable(sprite, {}, 3102);
        renderer.currentTransform.translate(rtl ? -8 : 56, 24);
        renderer.pushDrawable(powerupLabels[i], {}, 3103);
        renderer.currentTransform.translate(rtl ? 8 : -56, -24);
        i++;
        if (i == 5)
            renderer.currentTransform.translate(rtl ? -StandardSpacing : StandardSpacing, -224);
        else if (i != 11) renderer.currentTransform.translate(0, 56);
        if (!showSecretPowerups && i == 10) break;
        else if (i == 10 || i == 11)
            renderer.currentTransform.translate(rtl ^ (i == 11) ? StandardSpacing : -StandardSpacing, 0);
    }

    renderer.currentTransform.translate(rtl ?
        PlayfieldWidth/2 - (float)ScreenWidth + 148 : -(float)PlayfieldWidth/2 - 100, 0);

    renderer.currentTransform.translate(ScreenWidth/2, 20);
    renderer.pushDrawable(titles[1], {}, 3101);
    renderer.currentTransform.translate(-(float)ScreenWidth/2, 24);
    
    renderer.currentTransform.translate(rtl ? ScreenWidth - 156 : 108, 4);
    
    i = 0;
    for (auto& sprite : goldenTokenSprites)
    {
        renderer.pushDrawable(sprite, {}, 3102);
        i++;
        if (i % 3 == 0 && i < 30) renderer.currentTransform.translate(rtl ? -56 : 56, -112);
        else renderer.currentTransform.translate(0, 56);
    }

    renderer.currentTransform.translate(rtl ? 660 - (float)ScreenWidth : -612, 0);

    renderer.currentTransform.translate(ScreenWidth/2, 20);
    renderer.pushDrawable(titles[2], {}, 3101);
    renderer.currentTransform.translate(-(float)ScreenWidth/2, 24);

    renderer.currentTransform.translate(rtl ? ScreenWidth - 156 : 108, 22);

    auto drawPicket = [=, &renderer](size_t i)
    {
        float angle = 360 * toSeconds<float>(curTime - initTime) / toSeconds<float>(RotationPeriod);

        renderer.pushTransform();
        renderer.currentTransform.rotate(rtl ? -angle : angle);
        renderer.pushDrawable(picketSprites[i], {}, 3103);
        renderer.popTransform();
    };

    constexpr float LabelOffset = PlayfieldWidth/2 - 24;
    for (i = 0; i < 10;)
    {
        renderer.pushDrawable(levelLabels[i], {}, 3102);
        renderer.currentTransform.translate(rtl ? -LabelOffset + 40 : LabelOffset - 40, 0);
        renderer.pushDrawable(picketCount[i], {}, 3103);
        renderer.currentTransform.translate(rtl ? -22 : 22, 0);
        drawPicket(i);
        renderer.currentTransform.translate(rtl ? LabelOffset - 18 : -LabelOffset + 18, 0);
        i++;
        if (i == 5)
            renderer.currentTransform.translate(rtl ? -StandardSpacing : StandardSpacing, -176);
        else renderer.currentTransform.translate(0, 44);
    }

    renderer.currentTransform.translate(rtl ? StandardSpacing : -StandardSpacing, 0);

    renderer.currentTransform.translate(rtl ? StandardSpacing + LabelOffset : 0, 0);
    renderer.pushDrawable(totalLabel, {}, 3102);
    renderer.currentTransform.translate(rtl ?
        -StandardSpacing - LabelOffset + 40 : StandardSpacing + LabelOffset - 40, 0);
    renderer.pushDrawable(totalPicketCount, {}, 3102);
    renderer.currentTransform.translate(rtl ? -22 : 22, 0);
    drawPicket(10);
    
    renderer.popTransform();

    renderer.pushDrawable(ScissorRect::pop(), {}, 3104);

    renderer.currentTransform.translate(0, -64);
    scrollBar.render(renderer);
    renderer.currentTransform.translate(0, 64);
}

void CollectedPauseFrame::activate()
{
    
}
void CollectedPauseFrame::deactivate()
{
    
}
