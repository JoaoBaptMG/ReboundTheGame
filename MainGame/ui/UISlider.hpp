#pragma once

#include "UIButton.hpp"

#include "language/LangID.hpp"
#include "input/CommonActions.hpp"
#include "settings/InputSettings.hpp"

class ResourceManager;
class LocalizationManager;

class UISlider final : public UIButton
{
    Sprite sliderBody, sliderKnob;
    AxisAction slideAxis;
    
    InputManager::MouseMoveEntry sliderMoveEntry;
    
    size_t* slideTarget;
    size_t slideMaximum;
    std::function<void(size_t)> slideAction;
    sf::Vector2i mousePosition;
    
public:
    UISlider(size_t* target = nullptr, size_t max = 0) : UIButton(), slideTarget(target), slideMaximum(max) {}
    explicit UISlider(InputManager& inputManager, ResourceManager& resourceManager, const InputSettings& settings,
        size_t* target = nullptr, size_t max = 0);
    void initialize(InputManager& inputManager, ResourceManager& resourceManager, const InputSettings& settings);
    virtual ~UISlider() {}
    
    auto getSlideTarget() const { return slideTarget; }
    void setSlideTarget(size_t* target) { slideTarget = target; }
    
    auto getSlideMaximum() const { return slideTarget; }
    void setSlideMaximum(size_t max) { slideMaximum = max; }
    
    auto getSlideAction() const { return slideAction; }
    void setSlideAction(decltype(slideAction) action) { slideAction = action; }
    
    void resetKnob();
    
    void update();
    virtual void render(Renderer &renderer) override;
};

