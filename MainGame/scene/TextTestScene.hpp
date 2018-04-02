#pragma once

#include "Scene.hpp"
#include <SFML/Graphics.hpp>

#include "resources/ResourceManager.hpp"
#include "resources/FontHandler.hpp"
#include "drawables/TextDrawable.hpp"
#include <memory>

class Renderer;

class TextTestScene final : public Scene
{
    std::unique_ptr<FontHandler> fontHandler;
    FrameTime initTime;
    TextDrawable testText;
    ResourceManager& resourceManager;
    
    size_t curDemo;
    
public:
    TextTestScene(ResourceManager& manager);
    void setupDemo();
    
    virtual ~TextTestScene() {}
    
    virtual void update(FrameTime curTime) override;
    virtual void render(Renderer &renderer) override;
};
