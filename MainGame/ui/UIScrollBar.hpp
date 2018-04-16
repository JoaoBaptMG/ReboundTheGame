//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//


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
