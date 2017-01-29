#pragma once

#include "drawables/Tilemap.hpp"
#include "data/RoomData.hpp"

#include <cppmunk/Shape.h>

class GameScene;
class ResourceManager;
class Renderer;

class Room final : util::non_copyable
{
    std::shared_ptr<RoomData> currentRoom;
    std::vector<std::shared_ptr<Chipmunk::Shape>> roomShapes;

    Tilemap mainLayerTilemap;

    GameScene& gameScene;

public:
    explicit Room(GameScene &scene);
    ~Room();

    void loadRoom(std::string resourceName);

    void update(float delta);
    void render(Renderer& renderer);

    void generateRoomShapes();
    void clearShapes();

    size_t getWidth() const { return currentRoom->mainLayer.width(); }
    size_t getHeight() const { return currentRoom->mainLayer.height(); }
};

