#pragma once

#include <SFML/Graphics.hpp>
#include <cppmunk/Shape.h>
#include <cppmunk/Body.h>

class PlayerController;
class ResourceManager;
class GameScene;
class Renderer;

class Player final
{
    const PlayerController& controller;

    sf::Sprite sprite;
    std::shared_ptr<Chipmunk::Shape> playerShape;

public:
    Player(const PlayerController& controller, GameScene &scene);
    ~Player();

    void update(float dt);
    void render(Renderer& renderer);

    auto getPosition() const { return playerShape->getBody()->getPosition(); }

    auto getDisplayPosition() const
    {
        auto vec = getPosition();
        return sf::Vector2f(std::floor(vec.x), std::floor(vec.y));
    }
};

