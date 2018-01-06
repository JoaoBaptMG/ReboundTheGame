#pragma once

#include "UIButton.hpp"

#include "language/LangID.hpp"
#include "input/CommonActions.hpp"
#include "settings/InputSettings.hpp"

class ResourceManager;
class LocalizationManager;

class UITextSwitch final : public UIButton
{
    TextDrawable switchCaption;
    std::string trueString, falseString;
    
    AxisAction switchAxis;
    
    bool* switchTarget;
    std::function<void(bool)> switchAction;
    
public:
    UITextSwitch(bool* target = nullptr) : UIButton(), switchTarget(target) {}
    explicit UITextSwitch(InputManager& inputManager, const InputSettings& settings, bool* target = nullptr);
    void initialize(InputManager& inputManager, const InputSettings& settings);
    virtual ~UITextSwitch() {}
    
    auto getSwitchTarget() const { return switchTarget; }
    void setSwitchTarget(bool* target) { switchTarget = target; resetText(); }
    
    auto getSwitchAction() const { return switchAction; }
    void setSwitchAction(decltype(switchAction) action) { switchAction = action; }
    
    auto& getSwitchCaption() { return switchCaption; }
    const auto& getSwitchCaption() const { return switchCaption; }
    
    const auto& getTrueString() const { return trueString; }
    void setTrueString(std::string str) { trueString = str; resetText(); }
    
    const auto& getFalseString() const { return falseString; }
    void setFalseString(std::string str) { falseString = str; resetText(); }
    
    void resetText();
    
    virtual void render(Renderer &renderer) override;
};

void createCommonTextSwitch(UITextSwitch& button, ResourceManager& rm, LocalizationManager& lm,
    std::string activeResourceName, std::string pressedResourceName, sf::FloatRect centerRect,
    sf::FloatRect destRect, LangID captionString, size_t captionSize, sf::Color textColor,
    float outlineThickness, sf::Color outlineColor, sf::Vector2f captionDisplacement);
