#include "UIPointer.hpp"
#include <limits>
#include <cmath>

#include "resources/ResourceManager.hpp"
#include "rendering/Renderer.hpp"

UIPointer::UIPointer(InputManager& im, ResourceManager& rm)
    : pointer(rm.load<sf::Texture>("ui-pointer.png"), sf::Vector2f(0, 0)), position(NAN, NAN)
{
    callbackEntry = im.registerMouseMoveCallback([=] (sf::Vector2i position)
    {
        this->position = sf::Vector2f(position);
    });
}

void UIPointer::hide()
{
    position = sf::Vector2f(NAN, NAN);
}

void UIPointer::render(Renderer& renderer)
{
    if (std::isnan(position.x) || std::isnan(position.y)) return;
    
    renderer.pushTransform();
    renderer.currentTransform.translate(position);
    renderer.pushDrawable(pointer, {}, 150000);
    renderer.popTransform();
}

