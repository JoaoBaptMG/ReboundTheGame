#pragma once

#include "drawables/Tilemap.hpp"
#include "data/RoomData.hpp"
#include "objects/Player.hpp"

#include <cppmunk/Shape.h>
#include <chrono>

class GameScene;
class ResourceManager;
class Renderer;
class GameObject;

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

    void update(std::chrono::steady_clock::time_point curTime);
    void render(Renderer& renderer);

    void generateRoomShapes();
    void clearShapes();

    size_t getWidth() const { return currentRoom->mainLayer.width(); }
    size_t getHeight() const { return currentRoom->mainLayer.height(); }

    static constexpr cpCollisionType terrainCollisionType = 'rtrn';
    static constexpr cpCollisionType groundTerrainCollisionType = 'tgnd';
    static constexpr cpCollisionType wallTerrainCollisionType = 'twal';
};

