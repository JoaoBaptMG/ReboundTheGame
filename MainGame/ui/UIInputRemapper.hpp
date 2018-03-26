#pragma once

#include "UIButton.hpp"
#include "UIButtonGroup.hpp"

#include "language/LangID.hpp"
#include "input/CommonActions.hpp"
#include "settings/InputSettings.hpp"

class ResourceManager;
class LocalizationManager;

class UIInputRemapper final : public UIButton
{
    TextDrawable sourceCaption;
    InputSource& remapSource;
    LocalizationManager& localizationManager;
    bool forJoystick;

public:
    explicit UIInputRemapper(InputSource& source, LocalizationManager& lm, bool forJoystick) : UIButton(),
        remapSource(source), localizationManager(lm), forJoystick(forJoystick) {}
    UIInputRemapper(InputManager& inputManager, InputSource& source, LocalizationManager& lm, bool forJoystick);
    void initialize(InputManager& inputManager);
    virtual ~UIInputRemapper() {}
    
    auto& getSourceCaption() { return sourceCaption; }
    const auto& getSourceCaption() const { return sourceCaption; }
    
    void resetText();
    void resetRemappingState(InputManager& inputManager);
    
    virtual void render(Renderer &renderer) override;
};

void createCommonInputRemapper(UIInputRemapper& button, ResourceManager& rm, LocalizationManager& lm,
    std::string activeResourceName, std::string pressedResourceName, sf::FloatRect centerRect,
    sf::FloatRect destRect, LangID captionString, size_t captionSize, sf::Color textColor,
    float outlineThickness, sf::Color outlineColor, sf::Vector2f captionDisplacement);

class UIInputRemappingButtonGroup final : public UIButtonGroup
{
    UIInputRemapper* currentRemappingButton;
    InputSource* sourceCollection;
    size_t sourceCollectionSize;
    InputManager& inputManager;

public:
    UIInputRemappingButtonGroup(InputManager& inputManager, const InputSettings& settings, TravelingMode travelingMode =
        TravelingMode::Vertical);

    void setSourceCollection(InputSource* collection, size_t size);
    void setRemappingButton(UIInputRemapper* newRemappingButton);
    void assignRemappingUniquely(InputSource& curSource, InputSource newSource);
};
