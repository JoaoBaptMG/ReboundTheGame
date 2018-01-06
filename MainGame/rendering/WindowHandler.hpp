#pragma once

#include "Renderer.hpp"
#include <SFML/Graphics.hpp>
#include <non_copyable_movable.hpp>

class WindowHandler : util::non_copyable_movable
{
    sf::RenderWindow renderWindow;
    std::unique_ptr<sf::RenderTexture> fullscreenTexture;
    sf::VertexArray fullscreenQuad;
    Renderer renderer;
    
    bool vsyncEnabled;
    
    void enableFullscreen();
    void disableFullscreen();
    
public:
    WindowHandler(bool fullscreen, bool vsync);
    
    void setFullscreen(bool fullscreen);
    auto getFullscreen() const { return (bool)fullscreenTexture; }
    
    void setVsyncEnabled(bool vsync) { vsyncEnabled = vsync; renderWindow.setVerticalSyncEnabled(vsync); }
    
    Renderer& getRenderer() { return renderer; }
    sf::Window& getWindow() { return renderWindow; }
    
    void prepareForDraw();
    void display();
};
