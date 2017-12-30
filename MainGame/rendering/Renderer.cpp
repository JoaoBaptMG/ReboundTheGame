#include "Renderer.hpp"

Renderer::Renderer(Renderer&& other) noexcept : Renderer()
{
    swap(*this, other);
}
    
Renderer& Renderer::operator=(Renderer other)
{
    swap(*this, other);
    return *this;
}

void swap(Renderer& r1, Renderer& r2)
{
    using std::swap;
    
    swap(r1.drawableList, r2.drawableList);
    swap(r1.transformStack, r2.transformStack);
    swap(r1.currentTransform, r2.currentTransform);
}

void Renderer::pushDrawable(const sf::Drawable &drawable, sf::RenderStates states, long depth)
{
    states.transform.combine(currentTransform);
    drawableList.emplace(depth, std::make_pair(std::cref(drawable), states));
}

void Renderer::render(sf::RenderTarget& target)
{
    for (const auto& pair : drawableList)
        target.draw(pair.second.first, pair.second.second);
}

void Renderer::clearState()
{
    drawableList.clear();

    while (!transformStack.empty())
        transformStack.pop();

    currentTransform = sf::Transform::Identity;
}

void Renderer::pushTransform()
{
    transformStack.push(currentTransform);
}

void Renderer::popTransform()
{
    currentTransform = transformStack.top();
    transformStack.pop();
}
