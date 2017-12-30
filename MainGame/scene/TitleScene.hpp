#pragma once

#include "Scene.hpp"

#include "drawables/Sprite.hpp"
#include "ui/UIButton.hpp"
//#include "ui/UIButtonGroup.hpp"
#include "ui/UIPointer.hpp"

struct Settings;
class ResourceManager;
class LocalizationManager;

class TitleScene : public Scene
{
    Sprite background, foreground;
    UIButton buttons[4];
    UIPointer pointer;
    //UIButtonGroup buttonGroup;
    
public:
    TitleScene(const Settings& settings, InputManager& inputManager, ResourceManager& rm, LocalizationManager& lm);
    virtual ~TitleScene() {}
    
    virtual void update(std::chrono::steady_clock::time_point curTime) override;
    virtual void render(Renderer &renderer) override;
};
