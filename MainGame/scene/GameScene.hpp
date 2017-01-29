#pragma once

#include "scene/Scene.hpp"
#include "objects/Room.hpp"

#include <cppmunk/Space.h>
#include <SFML/Graphics.hpp>

#define CP_DEBUG 0

#if CP_DEBUG
#include "drawables/ChipmunkDebugDrawable.hpp"
#endif

class Player;
class Renderer;
class RoomData;
class ResourceManager;

class GameScene : public Scene
{
#if CP_DEBUG
    ChipmunkDebugDrawable debug;
#endif
    
    Chipmunk::Space gameSpace;
    Room room;

    ResourceManager &resourceManager;
    Player* currentPlayer;

    sf::Sprite guiLeft, guiRight;

public:
    GameScene(ResourceManager& manager);
    virtual ~GameScene() {}

    Chipmunk::Space& getGameSpace() { return gameSpace; }
    const Chipmunk::Space& getGameSpace() const { return gameSpace; }

    Room& getRoom() { return room; }
    const Room& getRoom() const { return room; }

    void setPlayer(Player& player) { currentPlayer = &player; }
    
    ResourceManager& getResourceManager() const { return resourceManager; }

    virtual void update(float dt) override;
    virtual void render(Renderer& renderer) override;
};

