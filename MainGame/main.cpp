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
#include <chronoUtils.hpp>

#if _WIN32
#include <Windows.h>
#define GL_PROGRAM_POINT_SIZE 0x8642
#define GL_POINT_SPRITE 0x8861
#endif

using namespace std::literals::chrono_literals;

int main(int argc, char **argv)
{
    sf::RenderWindow renderWindow(sf::VideoMode(ScreenWidth, ScreenHeight), "Game");
    renderWindow.setVerticalSyncEnabled(true);
    glEnable(GL_PROGRAM_POINT_SIZE);
#if _WIN32
	glEnable(GL_POINT_SPRITE);
#endif

    Renderer renderer(renderWindow);
    InputManager inputManager;
    PlayerController controller(inputManager);

    inputManager.setMapping(sf::Keyboard::Key::A, PlayerController::Actions::Left);
    inputManager.setMapping(sf::Keyboard::Key::D, PlayerController::Actions::Right);
    inputManager.setMapping(sf::Keyboard::Key::W, PlayerController::Actions::Up);
    inputManager.setMapping(sf::Keyboard::Key::S, PlayerController::Actions::Down);
    inputManager.setMapping((sf::Keyboard::Key)80, PlayerController::Actions::Jump);
    inputManager.setMapping((sf::Keyboard::Key)79, PlayerController::Actions::Dash);
    inputManager.setMapping((sf::Keyboard::Key)81, PlayerController::Actions::Bomb);

    ResourceManager manager;
    manager.setResourceLocator(new FilesystemResourceLocator());

    auto scene = new GameScene(manager);
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
            switch (event.type)
            {
                case sf::Event::EventType::Closed:
                    renderWindow.close();
                    break;
                case sf::Event::KeyPressed:
                    inputManager.handleKeyEvent(event.key.code, ActionState::Down);
                    break;
                case sf::Event::KeyReleased:
                    inputManager.handleKeyEvent(event.key.code, ActionState::Up);
                    break;
            }
        }

        auto curTime = std::chrono::steady_clock::now();

        while (curTime - updateTime > UpdateFrequency)
        {
            controller.update();
            updateTime += UpdateFrequency;
            sceneManager.update(updateTime);
        }

        renderWindow.clear(sf::Color::White);
        renderer.clearState();
        sceneManager.render(renderer);
        renderer.render();
        renderWindow.display();
    }

    return 0;
}
