#pragma once

#include "UIButton.hpp"

#include "drawables/Sprite.hpp"
#include "drawables/TextDrawable.hpp"

struct SavedGame;
class ResourceManager;
class LocalizationManager;

class UIFileSelectButton : public UIButton
{
    Sprite powerupSprites[10];
    Sprite goldenTokenSprite;
    Sprite picketSprite;
    bool rtl;
    
    TextDrawable fileName, goldenTokenAmount, picketAmount;
    
public:
    UIFileSelectButton(const SavedGame& sg, InputManager& im, ResourceManager& rm, LocalizationManager& lm, size_t k);
    virtual ~UIFileSelectButton() {}
    
    sf::FloatRect getBounds() const;
    
    virtual void render(Renderer &renderer) override;
};
