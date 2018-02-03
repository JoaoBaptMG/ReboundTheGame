#pragma once

#include "PauseFrame.hpp"

#include "drawables/TextDrawable.hpp"
#include "drawables/Sprite.hpp"

struct Settings;
class ResourceManager;
class InputManager;
class LocalizationManager;
struct SavedGame;

class CollectedPauseFrame : public PauseFrame
{
    TextDrawable titles[1];
    TextDrawable powerupLabels[10];
    
    Sprite powerupSprites[10];
    Sprite goldenTokenSprites[30];
    
    LocalizationManager& localizationManager;
    
public:
    CollectedPauseFrame(const Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager& lm);
    virtual ~CollectedPauseFrame() {}
    
    void setSavedGame(const SavedGame& savedGame);
    
    virtual void update(std::chrono::steady_clock::time_point curTime) override;
    virtual void render(Renderer &renderer) override;
    
    virtual void activate() override;
    virtual void deactivate() override;
};
