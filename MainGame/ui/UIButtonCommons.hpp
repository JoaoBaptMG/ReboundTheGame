#pragma once

#include "UIButton.hpp"

#include <string>
#include "language/LangID.hpp"

class ResourceManager;
class LocalizationManager;

void createCommonTextualButton(UIButton& button, ResourceManager& rm, LocalizationManager& lm,
    std::string activeResourceName, std::string pressedResourceName, sf::FloatRect centerRect,
    sf::FloatRect destRect, LangID captionString, size_t captionSize, sf::Color textColor,
    float outlineThickness, sf::Color outlineColor, sf::Vector2f captionDisplacement,
    TextDrawable::Alignment wordAlignment = TextDrawable::Alignment::Direct);

void createCommonGraphicsButton(UIButton& button, ResourceManager& rm, std::string normalResourceName,
    std::string activeResourceName, std::string pressedResourceName);
