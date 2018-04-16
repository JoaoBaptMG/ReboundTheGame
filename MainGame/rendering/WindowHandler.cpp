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


#include "WindowHandler.hpp"
#include "defaults.hpp"
#include <stdexcept>
#include <SFML/OpenGL.hpp>

#if _WIN32
#include <Windows.h>
#define GL_PROGRAM_POINT_SIZE 0x8642
#define GL_POINT_SPRITE 0x8861
#elif __APPLE__
#define GL_PROGRAM_POINT_SIZE 0x8642
#endif

void generateFullscreenQuad(sf::VertexArray& array, size_t canvasWidth, size_t canvasHeight)
{
    sf::FloatRect quadRect;

    if (ScreenWidth * canvasHeight < ScreenHeight * canvasWidth)
    {
        float ratio = ((float)ScreenWidth/ScreenHeight)*((float)canvasHeight/canvasWidth);
        quadRect = sf::FloatRect(canvasWidth * (1.0f - ratio) / 2, 0, canvasWidth * ratio, canvasHeight);
    }
    else if (ScreenWidth * canvasHeight > ScreenHeight * canvasWidth)
    {
        float ratio = ((float)ScreenHeight/ScreenWidth)*((float)canvasWidth/canvasHeight);
        quadRect = sf::FloatRect(0, canvasHeight * (1.0f - ratio) / 2, canvasWidth, canvasHeight * ratio);
    }
    else quadRect = sf::FloatRect(0, 0, canvasWidth, canvasHeight);

    array.setPrimitiveType(sf::PrimitiveType::TriangleFan);
    array.resize(4);
    
    for (size_t i = 0; i < 4; i++)
        array[i].color = sf::Color::White;
        
    array[0].texCoords = sf::Vector2f(0, 0);
    array[1].texCoords = sf::Vector2f(ScreenWidth, 0);
    array[2].texCoords = sf::Vector2f(ScreenWidth, ScreenHeight);
    array[3].texCoords = sf::Vector2f(0, ScreenHeight);
    
    array[0].position = sf::Vector2f(quadRect.left, quadRect.top);
    array[1].position = sf::Vector2f(quadRect.left + quadRect.width, quadRect.top);
    array[2].position = sf::Vector2f(quadRect.left + quadRect.width, quadRect.top + quadRect.height);
    array[3].position = sf::Vector2f(quadRect.left, quadRect.top + quadRect.height);
}

WindowHandler::WindowHandler(bool fullscreen, bool vsync) : vsyncEnabled(vsync)
{
    if (fullscreen) enableFullscreen();
    else disableFullscreen();
    
    renderWindow.setVerticalSyncEnabled(vsync);
    renderWindow.setKeyRepeatEnabled(false);
    renderWindow.setMouseCursorVisible(false);
}

void WindowHandler::setFullscreen(bool fullscreen)
{
    if (getFullscreen() == fullscreen) return;
    
    if (fullscreen) enableFullscreen();
    else disableFullscreen();
    
    glEnable(GL_PROGRAM_POINT_SIZE);
#if _WIN32
	glEnable(GL_POINT_SPRITE);
#endif

    renderWindow.setVerticalSyncEnabled(vsyncEnabled);
    renderWindow.setKeyRepeatEnabled(false);
    renderWindow.setMouseCursorVisible(false);
}

void WindowHandler::enableFullscreen()
{
    const auto& videoMode = sf::VideoMode::getFullscreenModes().front();
    
    renderWindow.create(videoMode, "Game", sf::Style::Fullscreen, sf::ContextSettings(24));
    
    fullscreenTexture = std::make_unique<sf::RenderTexture>();
    if (!fullscreenTexture->create(ScreenWidth, ScreenHeight, true))
        throw std::runtime_error("Failed to create the fullscreen render texture!");
        
    glEnable(GL_PROGRAM_POINT_SIZE);
#if _WIN32
	glEnable(GL_POINT_SPRITE);
#endif
        
    generateFullscreenQuad(fullscreenQuad, videoMode.width, videoMode.height);
    
    renderWindow.clear();
    renderWindow.display();
}

void WindowHandler::disableFullscreen()
{
    fullscreenTexture.reset();
    renderWindow.create(sf::VideoMode(ScreenWidth, ScreenHeight), "Game", sf::Style::Default & ~sf::Style::Resize,
        sf::ContextSettings(24));
        
    glEnable(GL_PROGRAM_POINT_SIZE);
#if _WIN32
	glEnable(GL_POINT_SPRITE);
#endif
}

void WindowHandler::prepareForDraw()
{
    renderer.clearState();
}

void WindowHandler::display()
{
    if (getFullscreen())
    {
        fullscreenTexture->clear();
        renderer.render(*fullscreenTexture);
        fullscreenTexture->display();
        
        //renderWindow.clear();
        renderWindow.draw(fullscreenQuad, sf::RenderStates(&fullscreenTexture->getTexture()));
        renderWindow.display();
    }
    else
    {
        renderWindow.clear();
        renderer.render(renderWindow);
        renderWindow.display();
    }
}
