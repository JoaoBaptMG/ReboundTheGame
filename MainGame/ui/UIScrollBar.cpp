#include "UIScrollBar.hpp"

#include <cmath>
#include <algorithm>
#include <assert.hpp>

#include "resources/ResourceManager.hpp"
#include "rendering/Renderer.hpp"

constexpr float ScrollSize = 8;

UIScrollBar::UIScrollBar(InputManager& im, ResourceManager& rm, intmax_t priority)
    : scrollRange(rm.load<sf::Texture>("ui-scroll-bar.png"), sf::Vector2f(0, 0)),
    scrollThumb(rm.load<sf::Texture>("ui-scroll-thumb.png"), sf::Vector2f(0, 0)),
    position(0,0), mousePosition(-1, -1), lastMousePos(-1, -1), dragging(false)
{
    scrollRange.setCenterRect(sf::FloatRect(3, 3, 2, 2));
    scrollThumb.setCenterRect(sf::FloatRect(3, 3, 2, 2));
    
    mouseEntry = im.registerMouseMoveCallback([=](sf::Vector2i pos)
    {
        lastMousePos = mousePosition;
        mousePosition = pos;
        
        if (dragging)
        {
            float delta = direction == Direction::Horizontal ? mousePosition.x - lastMousePos.x
                : mousePosition.y - lastMousePos.y;
            
            setCurrentOffset(curOffset + delta * maxOffset / (scrollLength - thumbSize + 1));
            
            sf::Vector2f floatMousePos(mousePosition.x, mousePosition.y);
            
            if (!isContainedOnStrip(floatMousePos - getPosition())) dragging = false;
        }
    }, priority);
    
    clickEntry = im.registerCallback(InputSource::mouseButton(sf::Mouse::Button::Left),
    [=](InputSource, float val)
    {
        sf::Vector2f floatMousePos(mousePosition.x, mousePosition.y);
        
        if (val > 0.5)
        {
            if (!dragging && getLocalThumbBounds().contains(floatMousePos - getPosition())) dragging = true;
        }
        else dragging = false;
        
    }, priority);
    
    wheelEntry = im.registerCallback(InputSource::mouseWheel(sf::Mouse::VerticalWheel),
    [=](InputSource, float val)
    { 
        setCurrentOffset(getCurrentOffset() - ScrollSize * val);
    }, priority);
    
    curOffset = 0;
}

UIScrollBar::UIScrollBar(InputManager& im, ResourceManager& rm, float viewSize, float scrollLength,
    UIScrollBar::Direction dir, intmax_t priority) : UIScrollBar(im, rm, priority)
{
    computeSizes(dir, viewSize, scrollLength);
}

void UIScrollBar::setCurrentOffset(float ofs)
{
    curOffset = std::max(0.0f, std::min(ofs, maxOffset));
    if (scrollAction) scrollAction(curOffset);
}

void UIScrollBar::computeSizes(UIScrollBar::Direction dir, float viewSize, float scrollLength)
{
    ASSERT(viewSize > scrollLength);
    
    direction = dir;
    
    curOffset = 0;
    maxOffset = viewSize - scrollLength;
    this->scrollLength = scrollLength;
    
    thumbSize = std::max(8.0f, std::round(scrollLength * scrollLength / viewSize));
    
    if (dir == Direction::Horizontal)
    {
        scrollRange.setDestinationRect(sf::FloatRect(0, 0, scrollLength, 8));
        scrollThumb.setDestinationRect(sf::FloatRect(0, 0, thumbSize, 8));
    }
    else
    {
        scrollRange.setDestinationRect(sf::FloatRect(0, 0, 8, scrollLength));
        scrollThumb.setDestinationRect(sf::FloatRect(0, 0, 8, thumbSize));
    }
}

sf::FloatRect UIScrollBar::getLocalThumbBounds() const
{
    sf::FloatRect bounds(getGraphicalOffset(), 0, thumbSize, 4);
    if (direction == Direction::Vertical)
    {
        using std::swap;
        swap(bounds.top, bounds.left);
        swap(bounds.width, bounds.height);
    }
    return bounds;
}

bool UIScrollBar::isContainedOnStrip(sf::Vector2f localPos) const
{
    auto bounds = getLocalThumbBounds();
    
    if (direction == Direction::Horizontal)
        return localPos.x >= bounds.left && localPos.x < bounds.left + bounds.width;
    else return localPos.y >= bounds.top && localPos.y < bounds.top + bounds.height;
}

float UIScrollBar::getGraphicalOffset() const
{
    return std::floor(curOffset * (scrollLength - thumbSize + 1) / maxOffset);
}

void UIScrollBar::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(position);
    renderer.pushDrawable(scrollRange, {}, depth);
    
    float pos = getGraphicalOffset();
    if (direction == Direction::Horizontal)
        renderer.currentTransform.translate(pos, 0);
    else renderer.currentTransform.translate(0, pos);
    renderer.pushDrawable(scrollThumb, {}, depth);
    
    renderer.popTransform();
}
