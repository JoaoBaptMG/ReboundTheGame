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

UIFileSelectButton::UIFileSelectButton(const SavedGame& sg, InputManager& im, ResourceManager& rm,
    LocalizationManager& lm, size_t index) : UIButton(im), rtl(lm.isRTL()),
    goldenTokenSprite(rm.load<sf::Texture>("golden-token.png")),
    picketSprite(rm.load<sf::Texture>("icon-picket.png")),
    fileName(rm.load<FontHandler>(lm.getFontName())),
    goldenTokenAmount(rm.load<FontHandler>(lm.getFontName())),
    picketAmount(rm.load<FontHandler>(lm.getFontName()))
{
    sf::FloatRect centerRect(4, 4, 4, 4);
    sf::FloatRect destRect(0, 0, ButtonSize, 128);
    sf::Vector2f destCenter(destRect.width/2, destRect.height/2);
    
    auto normalSprite = std::make_unique<SegmentedSprite>(rm.load<sf::Texture>("ui-file-button-frame.png"));
    normalSprite->setCenterRect(centerRect);
    normalSprite->setDestinationRect(destRect);
    normalSprite->setAnchorPoint(destCenter);
    
    auto activeSprite = std::make_unique<SegmentedSprite>(rm.load<sf::Texture>("ui-file-button-frame-active.png"));
    activeSprite->setCenterRect(centerRect);
    activeSprite->setDestinationRect(destRect);
    activeSprite->setAnchorPoint(destCenter);
    
    auto pressedSprite = std::make_unique<SegmentedSprite>(rm.load<sf::Texture>("ui-file-button-frame-pressed.png"));
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
        sprite.setTexture(rm.load<sf::Texture>("powerup" + std::to_string(k+1) + ".png"));
        bool is = sg.getAbilityLevel() > k;
        sprite.setBlendColor(sf::Color(is ? 255 : 0, is ? 255 : 0, is ? 255 : 0, 255));
        k++;
    }
    
    auto indexStr = lm.getFormattedString("file-select-index", {}, { { "i", index+1 } }, {});
    fileName.setFontHandler(rm.load<FontHandler>(lm.getFontName()));
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
    goldenTokenAmount.setFontHandler(rm.load<FontHandler>(lm.getFontName()));
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
    picketAmount.setFontHandler(rm.load<FontHandler>(lm.getFontName()));
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
