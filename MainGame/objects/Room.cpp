//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//


#include "Room.hpp"
#include "rendering/Renderer.hpp"
#include "scene/GameScene.hpp"
#include "resources/ResourceManager.hpp"
#include "objects/GameObject.hpp"
#include "gameplay/MapGenerator.hpp"
#include "particles/TextureExplosion.hpp"

#include <assert.hpp>

#include <functional>
#include <algorithm>
#include <cppmunk/Space.h>
#include <cppmunk/Body.h>
#include <cppmunk/SegmentShape.h>
#include <cppmunk/CircleShape.h>

std::vector<std::shared_ptr<cp::Shape>>
    generateShapesForTilemap(const RoomData& data, const TileSet& tileSet, std::shared_ptr<cp::Body> body,
    ShapeGeneratorDataOpaque& shaderGeneratorData, std::unordered_map<void*,CrumblingData>& crumblingTiles);

Room::Room(GameScene& scene) : gameScene(scene), shapeGeneratorData(nullptr, [](void*){}),
    transitionData(nullptr, [](void*){})
{
    util::rect drawingFrame{(float)(ScreenWidth-PlayfieldWidth)/2, (float)(ScreenHeight-PlayfieldHeight)/2,
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
        transitionDisplacement = glm::vec2(displacement.x, displacement.y);
        
        transitionShapes = std::move(roomShapes);
        transitionData = std::move(shapeGeneratorData);
        transitionBody = std::move(roomBody);
        transitionCrumblingTiles = std::move(crumblingTiles);
        
        transitionBody->setPosition(transitionBody->getPosition() + displacement);
        gameScene.getGameSpace().reindexShapesForBody(transitionBody);
    }
    
    tileSet = gameScene.getResourceManager().load<TileSet>(data.tilesetName + ".ts");
    
    mainLayerTilemap.setTexture(gameScene.getResourceManager().load<Texture>(tileSet->textureName));
    mainLayerTilemap.setTileData(data.mainLayer);

    clearShapes();
    roomBody = std::make_unique<cp::Body>(cp::Body::Static);
    roomShapes = generateShapesForTilemap(data, *tileSet, roomBody, shapeGeneratorData, crumblingTiles);
    for (auto& shape : roomShapes)
    {
        shape->setElasticity(0.6);
        shape->setCollisionType(CollisionType);
        gameScene.getGameSpace().add(shape);
    }
    gameScene.getGameSpace().add(roomBody);
}

void setCrumbleOffset(std::unique_ptr<TextureExplosion>& explosion, FrameDuration crumbleTime)
{
    explosion->setOffsetFunction([=](float x, float y)
    {
        using std::chrono::duration_cast;
        float factor = (1-y)/2;
        return duration_cast<TextureExplosion::Duration>(factor * crumbleTime);
    });
}

void Room::update(FrameTime curTime)
{
    auto checkCrumbling = [&,curTime,this](bool transition)
    {
        auto& roomBody = transition ? transitionBody : this->roomBody;
        auto& tilemap = transition ? transitionalTilemap : mainLayerTilemap;
        auto& crumblingTiles = transition ? transitionCrumblingTiles : this->crumblingTiles;

        if (!roomBody) return;

        roomBody->eachArbiter([&,curTime,this] (cp::Arbiter arbiter)
        {
            if (!cpShapeGetSensor(arbiter.getShapeA()) && !cpShapeGetSensor(arbiter.getShapeB()))
            {
                auto shp = cpShapeGetCollisionType(arbiter.getShapeA()) == CollisionType ?
                    arbiter.getShapeA() : arbiter.getShapeB();
                auto otherShp = cpShapeGetCollisionType(arbiter.getShapeA()) == CollisionType ?
                    arbiter.getShapeB() : arbiter.getShapeA();
                if (cpShapeGetCollisionType(otherShp) == CollisionType) return;

                void* attribute = cpShapeGetUserData(shp);
                if (*(TileSet::Attribute*)attribute == TileSet::Attribute::Crumbling)
                    if (isNull(crumblingTiles[attribute].initTime))
                        crumblingTiles[attribute].initTime = curTime;
            }
        });

        for (auto it = crumblingTiles.begin(); it != crumblingTiles.end();)
        {
            auto& data = it->second;
            if (isNull(data.initTime)) { ++it; continue; }

            if (!data.crumbling && curTime - data.initTime > data.waitTime)
            {
                auto grid = tilemap.getTileData();
                auto texRect = tilemap.getTextureRectForTile(grid(data.x, data.y));
                grid(data.x, data.y) = -1;
                tilemap.setTileData(grid);
                data.crumbling = true;

                auto grav = gameScene.getGameSpace().getGravity();
                auto displayGravity = glm::vec2((float)grav.x, (float)grav.y);
                
                auto explosion = std::make_unique<TextureExplosion>(gameScene, tilemap.getTexture(), texRect,
                    90_frames, util::rect(-64, 8, 128, 32), displayGravity, TextureExplosion::Density,
                    data.crumblePieceSize, data.crumblePieceSize, 25);
                explosion->setPosition((float)DefaultTileSize * glm::vec2(data.x + 0.5, data.y + 0.5));
                setCrumbleOffset(explosion, data.crumbleTime);
                gameScene.addObject(std::move(explosion));
            }
            
            if (curTime - data.initTime > data.waitTime + data.crumbleTime)
            {
                gameScene.getGameSpace().remove(data.shape);
                gameScene.getGameSpace().reindexShapesForBody(roomBody);
                it = crumblingTiles.erase(it);
            }
            else ++it;
        }
    };

    checkCrumbling(false);
    checkCrumbling(true);
}

void Room::render(Renderer& renderer, bool transition)
{
    renderer.pushDrawable(mainLayerTilemap, 25);
    
    if (transition)
    {
        renderer.pushTransform();
        renderer.currentTransform *= util::translate(transitionDisplacement);
        renderer.pushDrawable(transitionalTilemap, 25);
        renderer.popTransform();
    }
}

void Room::clearShapes()
{
    if (roomBody) gameScene.getGameSpace().remove(roomBody);
    roomBody = nullptr;
    
    for (auto shp : roomShapes)
    {
        if (gameScene.getGameSpace().contains(shp))
            gameScene.getGameSpace().remove(shp);
    }

    roomShapes.clear();
}

void Room::clearTransition()
{
    if (transitionBody) gameScene.getGameSpace().remove(transitionBody);
    transitionBody = nullptr;
    
    for (auto shp : transitionShapes)
    {
        if (gameScene.getGameSpace().contains(shp))
            gameScene.getGameSpace().remove(shp);
    }

    transitionShapes.clear();
    transitionData = ShapeGeneratorDataOpaque(nullptr, [](void*){});
    transitionCrumblingTiles.clear();
}
