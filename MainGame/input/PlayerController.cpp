#include "PlayerController.hpp"

PlayerController::PlayerController(InputManager &inputManager)
    : dash(active), jump(active), bomb(active), movement(active)
{
    dash.registerSource(inputManager, InputSource::keyboardKey((sf::Keyboard::Key)79));
    jump.registerSource(inputManager, InputSource::keyboardKey((sf::Keyboard::Key)80));
    bomb.registerSource(inputManager, InputSource::keyboardKey((sf::Keyboard::Key)81));

    movement.registerButtonForX(inputManager, InputSource::keyboardKey(sf::Keyboard::Key::A), AxisDirection::Negative);
    movement.registerButtonForX(inputManager, InputSource::keyboardKey(sf::Keyboard::Key::D), AxisDirection::Positive);
    movement.registerButtonForY(inputManager, InputSource::keyboardKey(sf::Keyboard::Key::W), AxisDirection::Negative);
    movement.registerButtonForY(inputManager, InputSource::keyboardKey(sf::Keyboard::Key::S), AxisDirection::Positive);
}

void PlayerController::update()
{
    dash.update();
    jump.update();
    bomb.update();
}
