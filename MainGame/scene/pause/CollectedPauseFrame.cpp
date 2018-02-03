#include "CollectedPauseFrame.hpp"

#include "resources/ResourceManager.hpp"
#include "gameplay/SavedGame.hpp"
#include "rendering/Renderer.hpp"
#include "language/LocalizationManager.hpp"
#include "language/convenienceConfigText.hpp"
#include "defaults.hpp"

const LangID Titles[] =
{
    "pause-collected-golden-tokens",
};

constexpr auto TitleSize = 32;
constexpr auto ButtonSize = 24;

CollectedPauseFrame::CollectedPauseFrame(const Settings& settings, InputManager& im, ResourceManager& rm,
    LocalizationManager& lm) : localizationManager(lm)
{
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
    for (auto& sprite : powerupSprites)
    {
        sprite.setTexture(rm.load<sf::Texture>("powerup" + std::to_string(i+1) + ".png"));
        i++;
    }
    
    for (auto& sprite : goldenTokenSprites)
        sprite.setTexture(rm.load<sf::Texture>("golden-token.png"));
}

void CollectedPauseFrame::setSavedGame(const SavedGame& savedGame)
{
    size_t i = 0;
    for (auto& sprite : powerupSprites)
    {
        bool is = savedGame.getAbilityLevel() > i;
        sprite.setBlendColor(sf::Color(is ? 255 : 0, is ? 255 : 0, is ? 255 : 0, 255));
        if (is)
        {
            powerupLabels[i].setString(localizationManager.getString("powerup" + std::to_string(i+1) + "-name"));
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
    
    i = 0;
    for (auto& sprite : goldenTokenSprites)
    {
        sprite.setGrayscaleFactor(!savedGame.getGoldenToken(i));
        i++;
    }
}

void CollectedPauseFrame::update(std::chrono::steady_clock::time_point curTime)
{
    
}

void CollectedPauseFrame::render(Renderer &renderer)
{
    bool rtl = localizationManager.isRTL();
    
    renderer.pushTransform();
    renderer.currentTransform.translate(ScreenWidth/2, 312);
    renderer.pushDrawable(titles[0], {}, 3000);
    renderer.popTransform();
    
    renderer.pushTransform();
    renderer.currentTransform.translate(rtl ? ScreenWidth - 156 : 108, 12);
    
    size_t i = 0;
    for (auto& sprite : powerupSprites)
    {
        renderer.pushDrawable(sprite, {}, 3100);
        renderer.currentTransform.translate(rtl ? -8 : 56, 24);
        renderer.pushDrawable(powerupLabels[i], {}, 3106);
        renderer.currentTransform.translate(rtl ? 8 : -56, -24);
        i++;
        if (i % 5 == 0)
            renderer.currentTransform.translate(rtl ? -(float)PlayfieldWidth/2 + 8 : PlayfieldWidth/2 - 8, -224);
        else renderer.currentTransform.translate(0, 56);
    }
    
    renderer.popTransform();
    
    renderer.pushTransform();
    renderer.currentTransform.translate(rtl ? ScreenWidth - 156 : 108, 340);
    
    i = 0;
    for (auto& sprite : goldenTokenSprites)
    {
        renderer.pushDrawable(sprite, {}, 3100);
        i++;
        if (i % 3 == 0) renderer.currentTransform.translate(rtl ? -56 : 56, -112);
        else renderer.currentTransform.translate(0, 56);
    }
    
    renderer.popTransform();
}

void CollectedPauseFrame::activate()
{

}
void CollectedPauseFrame::deactivate()
{
    
}
