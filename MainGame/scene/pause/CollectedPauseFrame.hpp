#pragma once

#include "PauseFrame.hpp"

#include "drawables/TextDrawable.hpp"
#include "drawables/Sprite.hpp"

#include "ui/UIScrollBar.hpp"

struct Settings;
class ResourceManager;
class InputManager;
class LocalizationManager;
struct SavedGame;

class CollectedPauseFrame : public PauseFrame
{
    UIScrollBar scrollBar;

    TextDrawable titles[3];
    TextDrawable powerupLabels[12];
    TextDrawable levelLabels[10], totalLabel;
    TextDrawable picketCount[10], totalPicketCount;
    
    Sprite powerupSprites[12];
    Sprite goldenTokenSprites[30];
    Sprite picketSprites[11];

    bool showSecretPowerups;

    LocalizationManager& localizationManager;
    FrameTime curTime, initTime;
    
public:
    size_t getTotalHeight() const;

    CollectedPauseFrame(const Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager& lm);
    virtual ~CollectedPauseFrame() {}

    void setHealthData(size_t curHealth, size_t maxHealth);
    void setSavedGame(const SavedGame& savedGame);
    
    virtual void update(FrameTime curTime) override;
    virtual void render(Renderer &renderer) override;
    
    virtual void activate() override;
    virtual void deactivate() override;
};
