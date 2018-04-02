#include "Room.hpp"
#include "rendering/Renderer.hpp"
#include "scene/GameScene.hpp"
#include "resources/ResourceManager.hpp"
#include "objects/GameObject.hpp"
#include "gameplay/MapGenerator.hpp"

#include <assert.hpp>

#include <functional>
#include <algorithm>
#include <cppmunk/Space.h>
#include <cppmunk/Body.h>
#include <cppmunk/SegmentShape.h>
#include <cppmunk/CircleShape.h>

std::vector<std::shared_ptr<cp::Shape>>
    generateShapesForTilemap(const RoomData& data, const TileSet& tileSet, std::shared_ptr<cp::Body> body,
    ShapeGeneratorDataOpaque& shaderGeneratorData);

Room::Room(GameScene& scene) : gameScene(scene), shapeGeneratorData(nullptr, [](void*){}),
    transitionData(nullptr, [](void*){})
{
    sf::FloatRect drawingFrame{(float)(ScreenWidth-PlayfieldWidth)/2, (float)(ScreenHeight-PlayfieldHeight)/2,
        (float)PlayfieldWidth, (float)PlayfieldHeight};
    mainLayerTilemap.setDrawingFrame(drawingFrame);
    transitionalTilemap.setDrawingFrame(drawingFrame);
}

Room::~Room()
{
    clearShapes();
    clearTransition();
}

void Room::loadRoom(const RoomData& data, bool transition, cpVect displacement)
{
    if (transition)
    {
        transitionalTilemap.setTexture(mainLayerTilemap.getTexture());
        transitionalTilemap.setTileData(mainLayerTilemap.getTileData());
        transitionDisplacement = sf::Vector2f(displacement.x, displacement.y);
        
        transitionShapes = std::move(roomShapes);
        transitionData = std::move(shapeGeneratorData);
        transitionBody = std::move(roomBody);
        
        transitionBody->setPosition(transitionBody->getPosition() + displacement);
        gameScene.getGameSpace().reindexShapesForBody(transitionBody);
    }
    
    tileSet = gameScene.getResourceManager().load<TileSet>(data.tilesetName + ".ts");
    
    mainLayerTilemap.setTexture(gameScene.getResourceManager().load<sf::Texture>(tileSet->textureName));
    mainLayerTilemap.setTileData(data.mainLayer);

    clearShapes();
    roomBody = std::make_unique<cp::Body>(cp::Body::Static);
    roomShapes = generateShapesForTilemap(data, *tileSet, roomBody, shapeGeneratorData);
    for (auto& shape : roomShapes)
    {
        shape->setElasticity(0.6);
        shape->setCollisionType(CollisionType);
        gameScene.getGameSpace().add(shape);
    }
    gameScene.getGameSpace().add(roomBody);
}

void Room::update(FrameTime curTime) {}

void Room::render(Renderer& renderer, bool transition)
{
    renderer.pushDrawable(mainLayerTilemap, {}, 25);
    
    if (transition)
    {
        renderer.pushTransform();
        renderer.currentTransform.translate(transitionDisplacement);
        renderer.pushDrawable(transitionalTilemap, {}, 25);
        renderer.popTransform();
    }
}

void Room::clearShapes()
{
    if (roomBody) gameScene.getGameSpace().remove(roomBody);
    roomBody = nullptr;
    
    for (auto shp : roomShapes)
        gameScene.getGameSpace().remove(shp);

    roomShapes.clear();
}

void Room::clearTransition()
{
    if (transitionBody) gameScene.getGameSpace().remove(transitionBody);
    transitionBody = nullptr;
    
    for (auto shp : transitionShapes)
        gameScene.getGameSpace().remove(shp);

    transitionShapes.clear();
    transitionData = ShapeGeneratorDataOpaque(nullptr, [](void*){});
}
