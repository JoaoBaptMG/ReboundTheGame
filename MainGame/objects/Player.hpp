#pragma once

#include <SFML/Graphics.hpp>
#include "../collision/CollisionHandler.hpp"

class PlayerController;
class ResourceManager;
class GameScene;
class Renderer;

class Player final
{
    const PlayerController& controller;

    sf::Sprite sprite;
    cp::CollisionBodyHandle handle;

public:
    Player(const PlayerController& controller, ResourceManager& resMan, GameScene &scene);
    ~Player();

    void update(float dt);
    void render(Renderer& renderer);

    cpVect getPosition() const { return cpBodyGetPosition(handle.getBodyPtr()); }

    sf::Vector2f getDisplayPosition() const
    {
        auto vec = getPosition();
        return sf::Vector2f(std::floor(vec.x), std::floor(vec.y));
    }
};

