#pragma once

#include "Scene.hpp"

#include "drawables/Sprite.hpp"
#include "drawables/TextDrawable.hpp"
#include "drawables/SegmentedSprite.hpp"
#include "ui/UIFileSelectButton.hpp"
#include "ui/UIButtonGroup.hpp"
#include "ui/UIPointer.hpp"
#include "ui/UIScrollBar.hpp"
#include <vector>
#include <memory>

struct Settings;
struct SavedGame;
class InputManager;
class ResourceManager;
class LocalizationManager;

class Renderer;

class FileSelectScene : public Scene
{
public:
    enum class FileAction { Load, Save };
    
private:
    Sprite sceneFrame;
    std::vector<std::unique_ptr<UIFileSelectButton>> fileButtons;
    std::unique_ptr<UIButton> dummyButton, substituteButton;
    std::unique_ptr<UIScrollBar> scrollBar;
    UIButton cancelButton;
    UIPointer pointer;
    UIButtonGroup buttonGroup;
    SegmentedSprite headerBackground;
    TextDrawable headerLabel;

public:
    FileSelectScene(Settings& settings, const SavedGame& savedGame, InputManager& im, ResourceManager& rm,
        LocalizationManager& lm, FileAction action);
    virtual ~FileSelectScene() {}
    
    virtual void update(FrameTime curTime) override;
    virtual void render(Renderer& renderer) override;
    
    size_t getScrollSize() const;
    void positionButton(size_t k);
};
