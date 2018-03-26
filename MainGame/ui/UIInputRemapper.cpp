#include "UIInputRemapper.hpp"

#include "UIButtonGroup.hpp"
#include "UIButtonCommons.hpp"

#include "rendering/Renderer.hpp"
#include <rectUtils.hpp>
#include <cmath>
#include <assert.h>

#include "resources/ResourceManager.hpp"
#include "language/LocalizationManager.hpp"
#include "language/convenienceConfigText.hpp"
#include "language/KeyboardKeyName.hpp"

UIInputRemapper::UIInputRemapper(InputManager& inputManager, InputSource& source, LocalizationManager& lm,
    bool forJoystick) : UIInputRemapper(source, lm, forJoystick)
{
    initialize(inputManager);
}

void UIInputRemapper::initialize(InputManager& inputManager)
{
    UIButton::initialize(inputManager);
    
    setPressAction([&inputManager,this] ()
    {
        auto parentGroup = static_cast<UIInputRemappingButtonGroup*>(this->parentGroup);
        parentGroup->setRemappingButton(this);

        auto callback = [=](InputSource source, float value)
        {
            if (value < 0.5f)
            {
                parentGroup->assignRemappingUniquely(remapSource, source);
                parentGroup->setRemappingButton(nullptr);
            }
        };

        if (forJoystick) inputManager.addPickAllJoystickCallback(callback);
        else inputManager.addPickAllKeyboardCallback(callback);

        sourceCaption.setString(localizationManager.getString("input-settings-remap"));
        sourceCaption.buildGeometry();
    });
}

void UIInputRemapper::resetText()
{
    sourceCaption.setString(remapSource.getInputName(localizationManager));
    sourceCaption.buildGeometry();
}

void UIInputRemapper::resetRemappingState(InputManager& inputManager)
{
    resetText();

    if (forJoystick) inputManager.addPickAllJoystickCallback(InputManager::Callback());
    else inputManager.addPickAllKeyboardCallback(InputManager::Callback());
}

void UIInputRemapper::render(Renderer& renderer)
{
    UIButton::render(renderer);
    
    renderer.pushTransform();
    renderer.currentTransform.translate(getPosition());

    auto bounds = getBounds();
    auto captionDisplacement = getCaptionDisplacement();
    
    renderer.currentTransform.translate(floorf(bounds.left + bounds.width/2), floorf(bounds.top + bounds.height/2));
    renderer.pushDrawable(sourceCaption, {}, getDepth());
    
    renderer.popTransform();
}

void createCommonInputRemapper(UIInputRemapper& button, ResourceManager& rm, LocalizationManager& lm,
    std::string activeResourceName, std::string pressedResourceName, sf::FloatRect centerRect,
    sf::FloatRect destRect, LangID captionString, size_t captionSize, sf::Color textColor,
    float outlineThickness, sf::Color outlineColor, sf::Vector2f captionDisplacement)
{
    createCommonTextualButton(button, rm, lm, activeResourceName, pressedResourceName, centerRect, destRect,
        captionString, captionSize, textColor, outlineThickness, outlineColor, captionDisplacement);
    
    auto& caption = button.getSourceCaption();
    caption.setFontHandler(rm.load<FontHandler>(lm.getFontName()));
    caption.setFontSize(captionSize);
    caption.setDefaultColor(textColor);
    caption.setOutlineThickness(outlineThickness);
    caption.setDefaultOutlineColor(outlineColor);
    caption.setHorizontalAnchor(TextDrawable::HorAnchor::Center);
    caption.setVerticalAnchor(TextDrawable::VertAnchor::Center);
    configTextDrawable(caption, lm);
    
    caption.setWordWrappingWidth(destRect.width - 2 * captionDisplacement.x);
    caption.setWordAlignment(TextDrawable::Alignment::Inverse);
    button.resetText();
}

UIInputRemappingButtonGroup::UIInputRemappingButtonGroup(InputManager& inputManager, const InputSettings& settings,
    TravelingMode travelingMode) : UIButtonGroup(inputManager, settings, travelingMode), inputManager(inputManager),
    currentRemappingButton(nullptr), sourceCollection(nullptr), sourceCollectionSize() {}

void UIInputRemappingButtonGroup::setSourceCollection(InputSource* collection, size_t size)
{
    sourceCollection = collection;
    sourceCollectionSize = size;
}

void UIInputRemappingButtonGroup::setRemappingButton(UIInputRemapper* newRemappingButton)
{
    if (currentRemappingButton == newRemappingButton) return;
    else if (currentRemappingButton != nullptr) currentRemappingButton->resetRemappingState(inputManager);
    currentRemappingButton = newRemappingButton;
}

void UIInputRemappingButtonGroup::assignRemappingUniquely(InputSource& curSource, InputSource newSource)
{
    for (size_t i = 0; i < sourceCollectionSize; i++)
        if (sourceCollection[i] == newSource)
        {
            sourceCollection[i] = curSource;

            for (auto button : getButtons())
            {
                auto btn = dynamic_cast<UIInputRemapper*>(button);
                if (btn) btn->resetText();
            }

            break;
        }

    curSource = newSource;
}
