#include "UIButtonCommons.hpp"

#include "resources/ResourceManager.hpp"
#include "resources/FontHandler.hpp"
#include "language/LocalizationManager.hpp"
#include "language/convenienceConfigText.hpp"
#include "drawables/Sprite.hpp"
#include "drawables/SegmentedSprite.hpp"

void createCommonTextualButton(UIButton& button, ResourceManager& rm, LocalizationManager& lm,
    std::string activeResourceName, std::string pressedResourceName, sf::FloatRect centerRect,
    sf::FloatRect destRect, LangID captionString, size_t captionSize, sf::Color textColor,
    float outlineThickness, sf::Color outlineColor, sf::Vector2f captionDisplacement,
    TextDrawable::Alignment wordAlignment)
{
    sf::Vector2f destCenter(destRect.width/2, destRect.height/2);
    
    auto activeSprite = std::make_unique<SegmentedSprite>(rm.load<sf::Texture>(activeResourceName));
    activeSprite->setCenterRect(centerRect);
    activeSprite->setDestinationRect(destRect);
    activeSprite->setAnchorPoint(destCenter);
    
    auto pressedSprite = std::make_unique<SegmentedSprite>(rm.load<sf::Texture>(pressedResourceName));
    pressedSprite->setCenterRect(centerRect);
    pressedSprite->setDestinationRect(destRect);
    pressedSprite->setAnchorPoint(destCenter);
    
    auto caption = std::make_unique<TextDrawable>(rm.load<FontHandler>(lm.getFontName()));
    if (!captionString.empty()) caption->setString(lm.getString(captionString));
    caption->setFontSize(captionSize);
    caption->setDefaultColor(textColor);
    caption->setOutlineThickness(outlineThickness);
    caption->setDefaultOutlineColor(outlineColor);
    caption->setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    caption->setVerticalAnchor(TextDrawable::VertAnchor::Center);
    caption->setWordWrappingWidth(destRect.width - 2 * captionDisplacement.x);
    caption->setWordAlignment(wordAlignment);
    configTextDrawable(*caption, lm);
    caption->buildGeometry();
    
    button.setActiveSprite(std::move(activeSprite));
    button.setPressedSprite(std::move(pressedSprite));
    button.setCaption(std::move(caption));
    button.setCaptionDisplacement(captionDisplacement);
    button.autoComputeBounds();
}

void createCommonGraphicsButton(UIButton& button, ResourceManager& rm, std::string normalResourceName,
    std::string activeResourceName, std::string pressedResourceName)
{
    
}
