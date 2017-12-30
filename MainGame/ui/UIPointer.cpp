#include "UIPointer.hpp"
#include <limits>

#include "resources/ResourceManager.hpp"
#include "rendering/Renderer.hpp"

UIPointer::UIPointer(InputManager& im, ResourceManager& rm)
    : pointer(rm.load<sf::Texture>("ui-pointer.png"), sf::Vector2f(0, 0))
{
    callbackEntry = im.registerMouseMoveCallback([=] (sf::Vector2i position)
    {
        this->position = sf::Vector2f(position);
    });
}

void UIPointer::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(position);
    renderer.pushDrawable(pointer, {}, std::numeric_limits<long>::max());
    renderer.popTransform();
}
