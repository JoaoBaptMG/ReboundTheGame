#pragma once

#include "../scene/Scene.hpp"
#include "../rendering/Renderer.hpp"
#include "../drawables/Tilemap.hpp"
#include "../data/RoomData.hpp"
#include "../resources/ResourceManager.hpp"
#include "../collision/CollisionBodyHandle.hpp"

class GameScene;

class Room final : util::non_copyable
{
    std::shared_ptr<RoomData> currentRoom;

    Tilemap mainLayerTilemap;

    GameScene& currentScene;
    ResourceManager& currentManager;

    cp::CollisionBodyHandle handle;

public:
    Room(GameScene &scene, ResourceManager& manager);
    ~Room();

    void loadRoom(std::string resourceName);

    void update(float delta);
    void render(Renderer& renderer);

    void generateRoomShapes();

    size_t getWidth() const { return currentRoom->mainLayer.width(); }
    size_t getHeight() const { return currentRoom->mainLayer.height(); }
};

