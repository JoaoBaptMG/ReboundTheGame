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

#include "ColorList.hpp"
#include "rendering/Texture.hpp"

#include "settings/Settings.hpp"
#include "settings/InputSettings.hpp"

const LangID Titles[] =
{
    "pause-collected-powerups",
    "pause-collected-golden-tokens",
    "pause-collected-pickets",
    "pause-collected-current-health",
};

constexpr auto TitleSize = 32;
constexpr auto ButtonSize = 24;

constexpr auto RotationPeriod = 72_frames;
constexpr float TravelAmount = 240;

constexpr float ContentSpacing = 12;
constexpr float ContentWidth = PlayfieldWidth - 2 * ContentSpacing;
constexpr float ColumnSpacing = 4;
constexpr float IconSpacing = 8;

static ScissorRectPush scissorRectPush{};

CollectedPauseFrame::CollectedPauseFrame(Services& services) : localizationManager(services.localizationManager),
    showSecretPowerups(false), scrollBar(services, 600, 512)
{
    scrollBar.setDepth(5100);
    scrollBar.setPosition(glm::vec2((ScreenWidth+PlayfieldWidth)/2 - 8, 64));

    size_t i = 0;
    for (auto& title : titles)
    {
        title.setFontHandler(loadDefaultFont(services));
        title.setString(services.localizationManager.getString(Titles[i]));
        title.setFontSize(TitleSize);
        title.setDefaultColor(Colors::Yellow);
        title.setOutlineThickness(1);
        title.setDefaultOutlineColor(Colors::Black);
        title.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
        title.setVerticalAnchor(TextDrawable::VertAnchor::Center);
        configTextDrawable(title, services.localizationManager);
        title.buildGeometry();
        i++;
    }

    if (services.localizationManager.isRTL())
        titles[3].setHorizontalAnchor(TextDrawable::HorAnchor::Right);
    else titles[3].setHorizontalAnchor(TextDrawable::HorAnchor::Left);
    titles[3].buildGeometry();

    for (auto& label : powerupLabels)
    {
        label.setFontHandler(loadDefaultFont(services));
        label.setString(services.localizationManager.getString("pause-collected-unknown"));
        label.setFontSize(ButtonSize);
        label.setDefaultColor(Colors::White);
        label.setOutlineThickness(1);
        label.setDefaultOutlineColor(Colors::Black);
        if (services.localizationManager.isRTL()) label.setHorizontalAnchor(TextDrawable::HorAnchor::Right);
        else label.setHorizontalAnchor(TextDrawable::HorAnchor::Left);
        label.setVerticalAnchor(TextDrawable::VertAnchor::Center);
        configTextDrawable(label, services.localizationManager);
        label.buildGeometry();
    }

    i = 0;
    for (auto& label : levelLabels)
    {
        label.setFontHandler(loadDefaultFont(services));
        label.setString(getLevelNameForNumber(services.localizationManager, i+1));
        label.setFontSize(ButtonSize);
        label.setDefaultColor(Colors::White);
        label.setOutlineThickness(1);
        label.setDefaultOutlineColor(Colors::Black);
        if (services.localizationManager.isRTL()) label.setHorizontalAnchor(TextDrawable::HorAnchor::Right);
        else label.setHorizontalAnchor(TextDrawable::HorAnchor::Left);
        label.setVerticalAnchor(TextDrawable::VertAnchor::Center);
        configTextDrawable(label, services.localizationManager);
        label.buildGeometry();
        i++;
    }

    for (auto& label : picketCount)
    {
        label.setFontHandler(loadDefaultFont(services));
        label.setString(services.localizationManager.getFormattedString("pause-collected-picket-count", {}, {{"n",0}}, {}));
        label.setFontSize(ButtonSize);
        label.setDefaultColor(Colors::White);
        label.setOutlineThickness(1);
        label.setDefaultOutlineColor(Colors::Black);
        if (services.localizationManager.isRTL()) label.setHorizontalAnchor(TextDrawable::HorAnchor::Left);
        else label.setHorizontalAnchor(TextDrawable::HorAnchor::Right);
        label.setVerticalAnchor(TextDrawable::VertAnchor::Center);
        configTextDrawable(label, services.localizationManager);
        label.buildGeometry();
    }

    i = 0;
    for (auto plabel : { &totalLabel, &totalPicketCount, &healthLabel })
    {
        auto& label = *plabel;
        label.setFontHandler(loadDefaultFont(services));
        if (i == 0) label.setString(services.localizationManager.getString("pause-collected-total"));
        else label.setString(services.localizationManager.getFormattedString("pause-collected-total-picket-count", {}, {{"n",0}}, {}));
        label.setFontSize(TitleSize);
        label.setDefaultColor(Colors::White);
        label.setOutlineThickness(1);
        label.setDefaultOutlineColor(Colors::Black);
        if (services.localizationManager.isRTL() ^ (i != 0)) label.setHorizontalAnchor(TextDrawable::HorAnchor::Right);
        else label.setHorizontalAnchor(TextDrawable::HorAnchor::Left);
        label.setVerticalAnchor(TextDrawable::VertAnchor::Center);
        configTextDrawable(label, services.localizationManager);
        label.buildGeometry();
        i++;
    }
    
    i = 0;
    for (auto& sprite : powerupSprites)
    {
        sprite.setTexture(services.resourceManager.load<Texture>("powerup" + std::to_string(i+1) + ".png"));
        sprite.setAnchorPoint(glm::vec2(sprite.getTextureSize()/2u));
        i++;
    }
    
    for (auto& sprite : goldenTokenSprites)
    {
        sprite.setTexture(services.resourceManager.load<Texture>("golden-token.png"));
        sprite.setAnchorPoint(glm::vec2(sprite.getTextureSize()/2u));
    }

    for (auto& sprite : picketSprites)
    {
        sprite.setTexture(services.resourceManager.load<Texture>("icon-picket.png"));
        sprite.setAnchorPoint(glm::vec2(sprite.getTextureSize()/2u));
        sprite.setBlendColor(Colors::Black);
    }

    auto& inputSettings = services.settings.inputSettings;
    travel.registerButton(services.inputManager, inputSettings.keyboardSettings.moveUp,    AxisDirection::Negative, 0);
    travel.registerButton(services.inputManager, inputSettings.keyboardSettings.moveDown,  AxisDirection::Positive, 0);
    travel.registerAxis(services.inputManager, inputSettings.joystickSettings.movementAxisY,    0);
    travel.registerAxis(services.inputManager, inputSettings.joystickSettings.movementAxisYAlt, 1);
}

void CollectedPauseFrame::setSavedGame(const SavedGame& savedGame)
{
    size_t i = 0;
    for (auto& sprite : powerupSprites)
    {
        bool is = i == 10 ? savedGame.getDoubleArmor() :
            i == 11 ? savedGame.getMoveRegen() :
            savedGame.getAbilityLevel() > i;

        sprite.setBlendColor(glm::u8vec4(is ? 255 : 0, is ? 255 : 0, is ? 255 : 0, 255));
        if (is)
        {
            auto name = "powerup" + std::to_string(i+1) + "-name";
            powerupLabels[i].setString(localizationManager.getString(name));
            powerupLabels[i].buildGeometry();
            
            constexpr float MaxWidth = ContentWidth/2 - 48 - IconSpacing - ColumnSpacing;
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
        picketSprites[i].setBlendColor(glm::u8vec4(b, b, b, 255));
        i++;
    }

    auto n = savedGame.getPicketCount();
    auto str = localizationManager.getFormattedString("pause-collected-total-picket-count", {}, {{"n",n}}, {});
    totalPicketCount.setString(str);
    totalPicketCount.buildGeometry();
    uint8_t b = 255 * n / 1000;
    picketSprites[10].setBlendColor(glm::u8vec4(b, b, b, 255));
}

void CollectedPauseFrame::setHealthData(size_t curHealth, size_t maxHealth)
{
    auto str = localizationManager.getFormattedString("pause-collected-health-label", {},
        {{"cur",curHealth},{"max",maxHealth}}, {});
    healthLabel.setString(str);
    healthLabel.buildGeometry();
}

size_t CollectedPauseFrame::getTotalHeight() const
{
    return 904 + (showSecretPowerups ? 56 : 0);
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
    constexpr float StandardSpacing = ContentWidth/2 + ColumnSpacing;
    constexpr float HealthSpacing = ContentWidth*2/3;

    bool rtl = localizationManager.isRTL();
    auto rtlInvert = [rtl](float x) { return rtl ? -x : x; };

    auto rect = util::rect((ScreenWidth - PlayfieldWidth)/2, 0, PlayfieldWidth, PlayfieldHeight);
    scissorRectPush = ScissorRectPush(renderer.currentTransform * rect);
    renderer.pushDrawable(scissorRectPush, 5600);

    renderer.pushTransform();

    renderer.currentTransform *= util::translate(ScreenWidth/2 + rtlInvert(-HealthSpacing/2),
        -scrollBar.getCurrentOffset() + 28);
    renderer.pushDrawable(titles[3], 5601);
    renderer.currentTransform *= util::translate(rtlInvert(HealthSpacing), 0);
    renderer.pushDrawable(healthLabel, 5601);

    renderer.currentTransform *= util::translate(rtlInvert(-HealthSpacing/2), 40);
    renderer.pushDrawable(titles[0], 5601);

    renderer.currentTransform *= util::translate(rtlInvert(-ContentWidth/2 + 24), 48 + IconSpacing);

    size_t i = 0;
    for (auto& sprite : powerupSprites)
    {
        renderer.pushDrawable(sprite, 5602);
        renderer.currentTransform *= util::translate(rtlInvert(24 + IconSpacing), 0);
        renderer.pushDrawable(powerupLabels[i], 5603);
        renderer.currentTransform *= util::translate(rtlInvert(-24 - IconSpacing), 0);
        i++;
        if (i == 5)
            renderer.currentTransform *= util::translate(rtlInvert(StandardSpacing), -224);
        else if (i != 11) renderer.currentTransform *= util::translate(0, 56);
        if (!showSecretPowerups && i == 10) break;
        else if (i == 10 || i == 11)
            renderer.currentTransform *= util::translate(rtl ^ (i == 11) ? StandardSpacing : -StandardSpacing, 0);
    }

    constexpr float TokenWidth = 480 + 27 * IconSpacing;
    renderer.currentTransform *= util::translate(rtlInvert(ContentWidth/2 - 24 - StandardSpacing), -4);
    renderer.pushDrawable(titles[1], 5601);
    renderer.currentTransform *= util::translate(rtlInvert(-TokenWidth/2 + 24), 48 + IconSpacing);

    i = 0;
    for (auto& sprite : goldenTokenSprites)
    {
        renderer.pushDrawable(sprite, 5602);
        i++;
        if (i % 3 == 0 && i < 30) renderer.currentTransform *= util::translate(rtlInvert(48 + 3 * IconSpacing), -112);
        else renderer.currentTransform *= util::translate(0, 48 + IconSpacing);
    }

    renderer.currentTransform *= util::translate(rtlInvert(-TokenWidth/2 + 24), -4);
    renderer.pushDrawable(titles[2], 5601);
    renderer.currentTransform *= util::translate(rtlInvert(-ContentWidth/2), 34 + IconSpacing);

    auto drawPicket = [=, &renderer](size_t i)
    {
        float angle = 2 * M_PI * toSeconds<float>(curTime - initTime) / toSeconds<float>(RotationPeriod);

        renderer.pushTransform();
        renderer.currentTransform *= util::rotate(rtl ? -angle : angle);
        renderer.pushDrawable(picketSprites[i], 5603);
        renderer.popTransform();
    };

    constexpr float LabelOffset = ContentWidth/2 - 12;
    for (i = 0; i < 10;)
    {
        renderer.pushDrawable(levelLabels[i], 5602);
        renderer.currentTransform *= util::translate(rtlInvert(LabelOffset - 40), 0);
        renderer.pushDrawable(picketCount[i], 5603);
        renderer.currentTransform *= util::translate(rtl ? -22 : 22, 0);
        drawPicket(i);
        renderer.currentTransform *= util::translate(rtlInvert(-LabelOffset + 18), 0);
        i++;
        if (i == 5)
            renderer.currentTransform *= util::translate(rtlInvert(StandardSpacing), -176);
        else renderer.currentTransform *= util::translate(0, 44);
    }

    renderer.currentTransform *= util::translate(rtlInvert(-StandardSpacing), 0);

    renderer.currentTransform *= util::translate(rtl ? StandardSpacing + LabelOffset : 0, 0);
    renderer.pushDrawable(totalLabel, 5602);
    renderer.currentTransform *= util::translate(rtlInvert(StandardSpacing + LabelOffset - 40), 0);
    renderer.pushDrawable(totalPicketCount, 5602);
    renderer.currentTransform *= util::translate(rtl ? -22 : 22, 0);
    drawPicket(10);
    
    renderer.popTransform();

    renderer.pushDrawable(ScissorRect::pop(), 5604);

    renderer.currentTransform *= util::translate(0, -64);
    scrollBar.render(renderer);
    renderer.currentTransform *= util::translate(0, 64);
}

void CollectedPauseFrame::activate()
{
    
}
void CollectedPauseFrame::deactivate()
{
    
}
