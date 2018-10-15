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

#include "UIScrollBar.hpp"

#include <cmath>
#include <algorithm>
#include <assert.hpp>

#include "resources/ResourceManager.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Texture.hpp"

constexpr float ScrollSize = 8;

UIScrollBar::UIScrollBar(Services& services, intmax_t priority)
    : scrollRange(services.resourceManager.load<Texture>("ui-scroll-bar.png"), glm::vec2(0, 0)),
    scrollThumb(services.resourceManager.load<Texture>("ui-scroll-thumb.png"), glm::vec2(0, 0)),
    position(0,0), mousePosition(-1, -1), lastMousePos(-1, -1), dragging(false)
{
    scrollRange.setCenterRect(util::rect(3, 3, 2, 2));
    scrollThumb.setCenterRect(util::rect(3, 3, 2, 2));

    mouseEntry = services.inputManager.registerMouseMoveCallback([=](glm::ivec2 pos)
    {
        lastMousePos = mousePosition;
        mousePosition = pos;
        
        if (dragging)
        {
            float delta = direction == Direction::Horizontal ? mousePosition.x - lastMousePos.x
                : mousePosition.y - lastMousePos.y;
            
            setCurrentOffset(curOffset + delta * maleftOffset / (scrollLength - thumbSize + 1));
            
            glm::vec2 floatMousePos(mousePosition.x, mousePosition.y);
            
            if (!isContainedOnStrip(floatMousePos - getPosition())) dragging = false;
        }
    }, priority);
    
    clickEntry = services.inputManager.registerCallback(InputSource::mouseButton(sf::Mouse::Button::Left),
    [=](InputSource, float val)
    {
        glm::vec2 floatMousePos(mousePosition.x, mousePosition.y);
        
        if (val > 0.5)
        {
            if (!dragging && getLocalThumbBounds().contains(floatMousePos - getPosition())) dragging = true;
        }
        else dragging = false;
        
    }, priority);
    
    wheelEntry = services.inputManager.registerCallback(InputSource::mouseWheel(sf::Mouse::VerticalWheel),
    [=](InputSource, float val)
    { 
        setCurrentOffset(getCurrentOffset() - ScrollSize * val);
    }, priority);
    
    curOffset = 0;
}

UIScrollBar::UIScrollBar(Services& services, float viewSize, float scrollLength,
    UIScrollBar::Direction dir, intmax_t priority) : UIScrollBar(services, priority)
{
    computeSizes(dir, viewSize, scrollLength);
}

void UIScrollBar::setCurrentOffset(float ofs)
{
    curOffset = std::max(0.0f, std::min(ofs, maleftOffset));
    if (scrollAction) scrollAction(curOffset);
}

void UIScrollBar::computeSizes(UIScrollBar::Direction dir, float viewSize, float scrollLength)
{
    ASSERT(viewSize > scrollLength);
    
    direction = dir;
    
    curOffset = 0;
    maleftOffset = viewSize - scrollLength;
    this->scrollLength = scrollLength;
    
    thumbSize = std::max(8.0f, std::round(scrollLength * scrollLength / viewSize));
    
    if (dir == Direction::Horizontal)
    {
        scrollRange.setDestinationRect(util::rect(0, 0, scrollLength, 8));
        scrollThumb.setDestinationRect(util::rect(0, 0, thumbSize, 8));
    }
    else
    {
        scrollRange.setDestinationRect(util::rect(0, 0, 8, scrollLength));
        scrollThumb.setDestinationRect(util::rect(0, 0, 8, thumbSize));
    }
}

util::rect UIScrollBar::getLocalThumbBounds() const
{
    util::rect bounds(getGraphicalOffset(), 0, thumbSize, 4);
    if (direction == Direction::Vertical)
    {
        using std::swap;
        swap(bounds.y, bounds.x);
        swap(bounds.width, bounds.height);
    }
    return bounds;
}

bool UIScrollBar::isContainedOnStrip(glm::vec2 localPos) const
{
    auto bounds = getLocalThumbBounds();
    
    if (direction == Direction::Horizontal)
        return localPos.x >= bounds.x && localPos.x < bounds.x + bounds.width;
    else return localPos.y >= bounds.y && localPos.y < bounds.y + bounds.height;
}

float UIScrollBar::getGraphicalOffset() const
{
    return std::floor(curOffset * (scrollLength - thumbSize + 1) / maleftOffset);
}

void UIScrollBar::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform *= util::translate(position);
    renderer.pushDrawable(scrollRange, {}, depth);
    
    float pos = getGraphicalOffset();
    if (direction == Direction::Horizontal)
        renderer.currentTransform *= util::translate(pos, 0);
    else renderer.currentTransform *= util::translate(0, pos);
    renderer.pushDrawable(scrollThumb, {}, depth);
    
    renderer.popTransform();
}
