#pragma once

#include <non_copyable_movable.hpp>

#include "input/InputManager.hpp"
#include "drawables/SegmentedSprite.hpp"

class ResourceManager;
class Renderer;

class UIScrollBar final : util::non_copyable
{
public:
    enum class Direction { Vertical, Horizontal };

private:
    SegmentedSprite scrollRange, scrollThumb;
    std::function<void(float)> scrollAction;
    Direction direction;
    float curOffset, maxOffset, scrollLength, thumbSize;
    sf::Vector2f position;
    sf::Vector2i mousePosition, lastMousePos;
    bool dragging;
    size_t depth;
    
    InputManager::MouseMoveEntry mouseEntry;
    InputManager::CallbackEntry clickEntry, wheelEntry;
    
public:
    UIScrollBar(InputManager& im, ResourceManager& rm, intmax_t priority = 0);
    UIScrollBar(InputManager& im, ResourceManager& rm, float viewSize, float scrollLength,
        Direction dir = Direction::Vertical, intmax_t priority = 0);
    ~UIScrollBar() {}
    
    auto getScrollAction() const { return scrollAction; }
    void setScrollAction(decltype(scrollAction) action) { scrollAction = action; }
    
    auto getCurrentOffset() const { return curOffset; }
    void setCurrentOffset(float ofs);
    
    auto getDirection() const { return direction; }
    auto getMaxOffset() const { return maxOffset; }
    auto getScrollLength() const { return scrollLength; }
    
    void computeSizes(Direction dir, float viewSize, float scrollLength);
    
    auto getPosition() const { return position; }
    void setPosition(sf::Vector2f pos) { position = pos; }
    
    auto getDepth() const { return depth; }
    void setDepth(size_t d) { depth = d; }

    sf::FloatRect getLocalThumbBounds() const;
    float getGraphicalOffset() const;
    bool isContainedOnStrip(sf::Vector2f localPos) const;
    
    void render(Renderer& renderer);
};
