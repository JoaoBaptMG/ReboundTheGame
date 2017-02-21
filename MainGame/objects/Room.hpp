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
    std::vector<std::shared_ptr<Chipmunk::Shape>> roomShapes;

    Tilemap mainLayerTilemap;
    GameScene& gameScene;

public:
    explicit Room(GameScene &scene);
    ~Room();

    void loadRoom(const RoomData& data);

    void update(std::chrono::steady_clock::time_point curTime);
    void render(Renderer& renderer);

    void generateRoomShapes(const RoomData& data);
    void clearShapes();

    static constexpr cpCollisionType TerrainCollisionType = 'rtrn';
    static constexpr cpCollisionType GroundTerrainCollisionType = 'tgnd';
    static constexpr cpCollisionType WallTerrainCollisionType = 'twal';
};

