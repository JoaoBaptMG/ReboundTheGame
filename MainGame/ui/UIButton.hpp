#pragma once

#include <memory>
#include <chrono>
#include <functional>
#include <string>
#include <non_copyable_movable.hpp>
#include "drawables/Sprite.hpp"
#include "drawables/TextDrawable.hpp"
#include "input/InputManager.hpp"

//class UIButtonGroup;
class Renderer;

class UIButton final : util::non_copyable
{
    std::unique_ptr<Sprite> normalSprite, activeSprite, pressedSprite;
    std::unique_ptr<TextDrawable> caption;
    sf::FloatRect bounds;
    sf::Vector2f position, captionDisplacement;
    //UIButtonGroup *parentGroup;
    enum class State { Normal, Active, Pressed } state;
    size_t depth;
    
    std::function<void()> pressAction, overAction, outAction;
    
    InputManager::CallbackEntry mouseButtonEntry;
    InputManager::MouseMoveEntry mouseMovedEntry;
    
public:
    UIButton() {}
    explicit UIButton(InputManager& inputManager);
    void initialize(InputManager& inputManager);
    ~UIButton() {}
    
    auto getNormalSprite() const { return normalSprite.get(); }
    void setNormalSprite(Sprite* sprite) { normalSprite.reset(sprite); }
    void setNormalSprite(std::unique_ptr<Sprite> &&sprite) { normalSprite = std::move(sprite); }
    
    auto getActiveSprite() const { return activeSprite.get(); }
    void setActiveSprite(Sprite* sprite) { activeSprite.reset(sprite); }
    void setActiveSprite(std::unique_ptr<Sprite> &&sprite) { activeSprite = std::move(sprite); }
    
    auto getPressedSprite() const { return pressedSprite.get(); }
    void setPressedSprite(Sprite* sprite) { pressedSprite.reset(sprite); }
    void setPressedSprite(std::unique_ptr<Sprite> &&sprite) { pressedSprite = std::move(sprite); }
    
    auto getCaption() const { return caption.get(); }
    void setCaption(TextDrawable* text) { caption.reset(text); }
    void setCaption(std::unique_ptr<TextDrawable> &&text) { caption = std::move(text); }
    
    auto getBounds() const { return bounds; }
    void setBounds(const sf::FloatRect& rect) { bounds = rect; }
    void autoComputeBounds();
    
    void render(Renderer &renderer);
    
    auto getPressAction() const { return pressAction; }
    void setPressAction(decltype(pressAction) act) { pressAction = act; }
    
    auto getOverAction() const { return overAction; }
    void setOverAction(decltype(overAction) act) { overAction = act; }
    
    auto getOutAction() const { return outAction; }
    void setOutAction(decltype(outAction) act) { outAction = act; }
    
    auto getDepth() const { return depth; }
    void setDepth(size_t d) { depth = d; }
    
    auto getPosition() const { return position; }
    void setPosition(sf::Vector2f pos) { position = pos; }
    
    auto getCaptionDisplacement() const { return captionDisplacement; }
    void setCaptionDisplacement(sf::Vector2f displacement) { captionDisplacement = displacement; }
    
    //friend class UIButtonGroup;
};


