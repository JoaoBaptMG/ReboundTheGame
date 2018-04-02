#pragma once

#include "Scene.hpp"

#include "drawables/Sprite.hpp"
#include "ui/UIButton.hpp"
#include "ui/UIButtonGroup.hpp"
#include "ui/UIPointer.hpp"

struct Settings;
class ResourceManager;
class LocalizationManager;

class TitleScene : public Scene
{
    Sprite background, foreground;
    UIButton buttons[4];
    UIPointer pointer;
    UIButtonGroup buttonGroup;
    bool rtl;
    
public:
    TitleScene(Settings& settings, InputManager& inputManager, ResourceManager& rm, LocalizationManager& lm);
    virtual ~TitleScene() {}
    
    virtual void update(FrameTime curTime) override;
    virtual void render(Renderer &renderer) override;
    
    virtual void pause() override;
    virtual void resume() override;
};
