#pragma once

#include "Scene.hpp"
#include "../collision/CollisionHandler.hpp"
#include "../objects/Room.hpp"
#include <SFML/Graphics.hpp>

class Player;
class Renderer;
class RoomData;
class ResourceManager;

class GameScene : public Scene
{
    CollisionHandler collisionHandler;
    Room room;

    Player* currentPlayer;

    sf::Sprite guiLeft, guiRight;

public:
    GameScene(ResourceManager& manager);
    virtual ~GameScene() {}

    CollisionHandler& getCollisionHandler() { return collisionHandler; }
    const CollisionHandler& getCollisionHandler() const { return collisionHandler; }

    Room& getRoom() { return room; }
    const Room& getRoom() const { return room; }

    void setPlayer(Player& player) { currentPlayer = &player; }

    virtual void update(float dt) override;
    virtual void render(Renderer& renderer) override;
};

