#include <iostream>
#include <chrono>
#include <thread>
#include <utility>
#include <SFML/Graphics.hpp>
#include "input/InputManager.hpp"
#include "input/PlayerController.hpp"
#include "scene/Scene.hpp"
#include "utility/assert.hpp"
#include "resources/ResourceManager.hpp"
#include "resources/FilesystemResourceLocator.hpp"
#include "drawables/Tilemap.hpp"
#include "utility/grid.hpp"
#include "scene/SceneManager.hpp"
#include "scene/GameScene.hpp"
#include "objects/Player.hpp"
#include "collision/chipmunkWrapperTypes.hpp"
#include "defaults.hpp"

using namespace std::literals::chrono_literals;

int main(int argc, char **argv)
{
    sf::RenderWindow renderWindow(sf::VideoMode(ScreenWidth, ScreenHeight), "Game");

    Renderer renderer(renderWindow);
    InputManager inputManager;
    PlayerController controller(inputManager);

    inputManager.setMapping(sf::Keyboard::Key::A, PlayerController::Actions::Left);
    inputManager.setMapping(sf::Keyboard::Key::D, PlayerController::Actions::Right);
    inputManager.setMapping(sf::Keyboard::Key::W, PlayerController::Actions::Up);
    inputManager.setMapping(sf::Keyboard::Key::S, PlayerController::Actions::Down);
    inputManager.setMapping((sf::Keyboard::Key)80, PlayerController::Actions::Jump);

    ResourceManager manager;
    manager.setResourceLocator(new FilesystemResourceLocator());

    auto scene = new GameScene(manager);
    scene->getRoom().loadRoom("room1.map");

    SceneManager sceneManager;
    sceneManager.pushScene(scene);

    Player player(controller, manager, *scene);

    scene->setPlayer(player);

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

        controller.update();
        sceneManager.update(0.016);

        renderWindow.clear(sf::Color::White);
        renderer.clearState();
        sceneManager.render(renderer);
        renderer.render();
        renderWindow.display();

        std::this_thread::sleep_for(16ms);
    }

    return 0;
}
