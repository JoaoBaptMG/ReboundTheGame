#include <iostream>
#include <chrono>
#include <thread>
#include <utility>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "input/InputManager.hpp"
#include "input/PlayerController.hpp"
#include "scene/Scene.hpp"
#include <assert.hpp>
#include "resources/ResourceManager.hpp"
#include "resources/FilesystemResourceLocator.hpp"
#include "drawables/Tilemap.hpp"
#include <grid.hpp>
#include "scene/SceneManager.hpp"
#include "scene/GameScene.hpp"
#include "objects/Player.hpp"
#include "rendering/Renderer.hpp"
#include "settings/Settings.hpp"
#include <chronoUtils.hpp>

#if _WIN32
#include <Windows.h>
#define GL_PROGRAM_POINT_SIZE 0x8642
#define GL_POINT_SPRITE 0x8861
#endif

using namespace std::literals::chrono_literals;

float configureViewForFullscreen(sf::RenderWindow& renderWindow);
std::string CurrentIcon = "player.png";
static std::string LastIcon = ""; // TODO: remove this hack

int main(int argc, char **argv)
{
    bool success;
    auto settings = loadSettingsFile(&success);
    if (!success) std::cout << "WARNING! Settings file not loaded properly!" << std::endl;
    
    sf::RenderWindow renderWindow(sf::VideoMode(ScreenWidth, ScreenHeight), "Game",
        settings.videoSettings.fullscreen ? sf::Style::Fullscreen : sf::Style::Default);
    renderWindow.setVerticalSyncEnabled(settings.videoSettings.vsyncEnabled);

    float scalingFactor = 1.0f;
    if (settings.videoSettings.fullscreen)
        scalingFactor = configureViewForFullscreen(renderWindow);
    
    glEnable(GL_PROGRAM_POINT_SIZE);
#if _WIN32
	glEnable(GL_POINT_SPRITE);
#endif

    Renderer renderer(renderWindow);
    renderer.windowScalingFactor = scalingFactor;
    InputManager inputManager;
    PlayerController controller(inputManager, settings.inputSettings);

    ResourceManager resourceManager;
    resourceManager.setResourceLocator(new FilesystemResourceLocator());

    auto scene = new GameScene(resourceManager);
    scene->setPlayerController(controller);
    scene->loadLevel("level1.lvl");

    SceneManager sceneManager;
    sceneManager.pushScene(scene);

    auto updateTime = std::chrono::steady_clock::now();

    while (renderWindow.isOpen())
    {
        sf::Event event;

        while (renderWindow.pollEvent(event))
        {
            if (inputManager.handleEvent(event)) continue;
            switch (event.type)
            {
                case sf::Event::EventType::Closed:
                    renderWindow.close();
                    break;
                default: break;
            }
        }

        auto curTime = std::chrono::steady_clock::now();

		while (curTime <= updateTime)
		{
			std::this_thread::sleep_until(updateTime);
			curTime = std::chrono::steady_clock::now();
		}

        while (curTime > updateTime)
        {
            controller.update();
            updateTime += UpdateFrequency;
            sceneManager.update(updateTime);
        }

        renderWindow.clear(sf::Color::Black);
        renderer.clearState();
        sceneManager.render(renderer);
        renderer.render();
        renderWindow.display();

		if (CurrentIcon != LastIcon)
		{
			if (!CurrentIcon.empty())
			{
				auto img = resourceManager.load<sf::Texture>(CurrentIcon)->copyToImage();
				renderWindow.setIcon(img.getSize().x, img.getSize().y, img.getPixelsPtr());
			}
			LastIcon = CurrentIcon;
		}
    }

    if (!storeSettingsFile(settings))
        std::cout << "WARNING! Settings file not stored properly!" << std::endl;

    return 0;
}

float configureViewForFullscreen(sf::RenderWindow& renderWindow)
{
    renderWindow.clear(sf::Color::Black);
    renderWindow.display();
    
    auto windowSize = renderWindow.getSize();
    auto screen = sf::Vector2u(ScreenWidth, ScreenHeight); 
    
    std::cout << windowSize.x << ' ' << windowSize.y << std::endl;

    sf::View view(sf::FloatRect(0.0, 0.0, (float)screen.x, (float)screen.y));
    float scalingFactor;
    if (screen.x * windowSize.y < screen.y * windowSize.x)
    {
        float ratio = ((float)screen.x/screen.y)*((float)windowSize.y/windowSize.x);
        view.setViewport(sf::FloatRect(0.5f - 0.5f * ratio, 0.0f, ratio, 1.0f));
        scalingFactor = (float)windowSize.y/screen.y;
    }
    else if (screen.x * windowSize.y > screen.y * windowSize.x)
    {
        float ratio = ((float)screen.y/screen.x)*((float)windowSize.x/windowSize.y);
        view.setViewport(sf::FloatRect(0.0f, 0.5f - 0.5f * ratio, 1.0f, ratio));
        scalingFactor = (float)windowSize.x/screen.x;
    }
    else scalingFactor = (float)windowSize.y/screen.y;
    renderWindow.setView(view);

    return scalingFactor;
}
