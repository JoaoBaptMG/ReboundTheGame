#pragma once

#include "drawables/Sprite.hpp"
#include "input/InputManager.hpp"

class ResourceManager;
class Renderer;

class UIPointer final
{
    Sprite pointer;
    sf::Vector2f position;
    InputManager::MouseMoveEntry callbackEntry;
    
public:
    UIPointer(InputManager& im, ResourceManager& rm);
    ~UIPointer() {}
    
    void render(Renderer& renderer);
};
