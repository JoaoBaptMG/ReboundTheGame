#pragma once

#include "PauseFrame.hpp"

#include "drawables/Sprite.hpp"

struct Settings;
class ResourceManager;
class InputManager;
class LocalizationManager;

class CollectedPauseFrame : public PauseFrame
{
public:
    CollectedPauseFrame(const Settings& settings, InputManager& im, ResourceManager& rm, LocalizationManager& lm);
    virtual ~CollectedPauseFrame() {}
    
    virtual void update(std::chrono::steady_clock::time_point curTime) override;
    virtual void render(Renderer &renderer) override;
    
    virtual void activate() override;
    virtual void deactivate() override;
};
