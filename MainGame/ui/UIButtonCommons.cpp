#include "UIButtonCommons.hpp"

#include "resources/ResourceManager.hpp"
#include "resources/FontHandler.hpp"
#include "language/LocalizationManager.hpp"
#include "drawables/Sprite.hpp"
#include "drawables/SegmentedSprite.hpp"

void createCommonTextualButton(UIButton& button, ResourceManager& rm, LocalizationManager& lm,
    std::string activeResourceName, std::string pressedResourceName, sf::FloatRect centerRect,
    sf::FloatRect destRect, LangID captionString, size_t captionSize, sf::Color textColor,
    float outlineThickness, sf::Color outlineColor, TextDrawable::HorAnchor horAnchor,
    TextDrawable::VertAnchor vertAnchor)
{
    auto activeSprite = std::make_unique<SegmentedSprite>(rm.load<sf::Texture>(activeResourceName));
    activeSprite->setCenterRect(centerRect);
    activeSprite->setDestinationRect(destRect);
    activeSprite->setAnchorPoint(sf::Vector2f(destRect.left, destRect.top)/2.0f);
    
    auto pressedSprite = std::make_unique<SegmentedSprite>(rm.load<sf::Texture>(pressedResourceName));
    pressedSprite->setCenterRect(centerRect);
    pressedSprite->setDestinationRect(destRect);
    pressedSprite->setAnchorPoint(sf::Vector2f(destRect.left, destRect.top)/2.0f);
    
    auto caption = std::make_unique<TextDrawable>(rm.load<FontHandler>(lm.getFontName()));
    caption->setString(lm.getString(captionString));
    caption->setFontSize(captionSize);
    caption->setDefaultColor(textColor);
    caption->setOutlineThickness(outlineThickness);
    caption->setDefaultOutlineColor(outlineColor);
    caption->setHorizontalAnchor(horAnchor);
    caption->setVerticalAnchor(vertAnchor);
    caption->buildGeometry();
    
    button.setActiveSprite(std::move(activeSprite));
    button.setPressedSprite(std::move(pressedSprite));
    button.setCaption(std::move(caption));
    button.autoComputeBounds();
}

void createCommonGraphicsButton(UIButton& button, ResourceManager& rm, std::string normalResourceName,
    std::string activeResourceName, std::string pressedResourceName)
{
}
