#pragma once

#include "drawables/Tilemap.hpp"
#include "data/RoomData.hpp"
#include "data/TileSet.hpp"
#include "objects/Player.hpp"

#include <cppmunk/Shape.h>
#include <chrono>

class GameScene;
class ResourceManager;
class Renderer;
class GameObject;

using ShapeGeneratorDataOpaque = std::unique_ptr<void, void(*)(void*)>;

class Room final : util::non_copyable
{
    GameScene& gameScene;
    
    std::vector<std::shared_ptr<cp::Shape>> roomShapes, transitionShapes;
    ShapeGeneratorDataOpaque shapeGeneratorData, transitionData;
    std::shared_ptr<cp::Body> roomBody, transitionBody;

    std::shared_ptr<TileSet> tileSet;
    Tilemap mainLayerTilemap;
    Tilemap transitionalTilemap;
    sf::Vector2f transitionDisplacement;

public:
    explicit Room(GameScene &scene);
    ~Room();

    void loadRoom(const RoomData& data, bool transition, cpVect displacement);

    void update(std::chrono::steady_clock::time_point curTime);
    void render(Renderer& renderer, bool transition);
    
    void clearTransition();

    void generateRoomShapes(const RoomData& data);
    void clearShapes();

    static constexpr cpCollisionType CollisionType = 'room';
};

