#pragma once

#include "Scene.hpp"

#include "drawables/Sprite.hpp"
#include "drawables/TextDrawable.hpp"
#include "ui/UIButton.hpp"
#include "ui/UIButtonGroup.hpp"
#include "ui/UIPointer.hpp"

struct Settings;
struct SavedGame;
class InputManager;
class ResourceManager;
class LocalizationManager;

class MidLevelScene : public Scene
{
    std::string nextLevel;

    Sprite sceneFrame;
    TextDrawable title;
    UIButton buttons[3];
    UIPointer pointer;
    UIButtonGroup buttonGroup;

public:
    MidLevelScene(Settings& settings, const SavedGame& sg, InputManager& im, ResourceManager& rm,
        LocalizationManager &lm, std::string nextLevel, bool gameover = true);
    virtual ~MidLevelScene() {}
    
    virtual void update(FrameTime curTime) override;
    virtual void render(Renderer &renderer) override;
};
