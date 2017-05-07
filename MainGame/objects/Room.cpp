#include "Room.hpp"
#include "rendering/Renderer.hpp"
#include "scene/GameScene.hpp"
#include "resources/ResourceManager.hpp"
#include "objects/GameObject.hpp"

#include <functional>
#include <algorithm>
#include <cppmunk/Space.h>
#include <cppmunk/Body.h>
#include <cppmunk/SegmentShape.h>
#include <cppmunk/CircleShape.h>

std::vector<std::shared_ptr<cp::Shape>>
    generateShapesForTilemap(const RoomData& data, const TileSet& tileSet, std::shared_ptr<cp::Body> body);

Room::Room(GameScene& scene) : gameScene(scene)
{
    sf::FloatRect drawingFrame{(ScreenWidth-PlayfieldWidth)/2, (ScreenHeight-PlayfieldHeight)/2,
        PlayfieldWidth, PlayfieldHeight};
    mainLayerTilemap.setDrawingFrame(drawingFrame);
}

Room::~Room()
{
    clearShapes();
}

void Room::loadRoom(const RoomData& data)
{
    tileSet = gameScene.getResourceManager().load<TileSet>(data.tilesetName + ".ts");
    
    mainLayerTilemap.setTexture(gameScene.getResourceManager().load<sf::Texture>(tileSet->textureName));
    mainLayerTilemap.setTileData(data.mainLayer);

    clearShapes();

    roomShapes = generateShapesForTilemap(data, *tileSet, gameScene.getGameSpace().getStaticBody());
    for (auto& shape : roomShapes)
    {
        shape->setElasticity(0.6);
        shape->setCollisionType(CollisionType);
        gameScene.getGameSpace().add(shape);
    }
}

void Room::update(std::chrono::steady_clock::time_point curTime) {}

void Room::render(Renderer& renderer)
{
    renderer.pushDrawable(mainLayerTilemap, {}, 25);
}

void Room::clearShapes()
{
    for (auto shp : roomShapes)
        gameScene.getGameSpace().remove(shp);

    roomShapes.clear();
}
