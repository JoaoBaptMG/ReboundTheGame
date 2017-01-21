#include "Renderer.hpp"

void Renderer::pushDrawable(const sf::Drawable &drawable, sf::RenderStates states, long depth)
{
    states.transform.combine(currentTransform);
    drawableList.emplace(depth, std::make_pair(std::cref(drawable), states));
}

void Renderer::render()
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
