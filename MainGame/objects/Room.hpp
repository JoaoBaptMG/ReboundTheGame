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

#pragma once

#include "drawables/Tilemap.hpp"
#include "data/RoomData.hpp"
#include "data/TileSet.hpp"
#include "objects/Player.hpp"

#include <cppmunk/Shape.h>
#include <cppmunk/Arbiter.h>
#include <chronoUtils.hpp>
#include <unordered_map>

class GameScene;
class ResourceManager;
class Renderer;
class GameObject;

using ShapeGeneratorDataOpaque = std::unique_ptr<void, void(*)(void*)>;

struct CrumblingData
{
    size_t x, y;
    std::shared_ptr<cp::Shape> shape;
    FrameDuration waitTime, crumbleTime;
    size_t crumblePieceSize;
    FrameTime initTime;
    bool crumbling;
};

class Room final : util::non_copyable
{
    GameScene& gameScene;
    
    std::vector<std::shared_ptr<cp::Shape>> roomShapes, transitionShapes;
    ShapeGeneratorDataOpaque shapeGeneratorData, transitionData;
    std::unordered_map<void*,CrumblingData> crumblingTiles, transitionCrumblingTiles;
    std::shared_ptr<cp::Body> roomBody, transitionBody;

    std::shared_ptr<TileSet> tileSet;
    Tilemap mainLayerTilemap;
    Tilemap transitionalTilemap;
    sf::Vector2f transitionDisplacement;

public:
    explicit Room(GameScene &scene);
    ~Room();

    void loadRoom(const RoomData& data, bool transition, cpVect displacement);

    void update(FrameTime curTime);
    void render(Renderer& renderer, bool transition);
    
    void clearTransition();

    void generateRoomShapes(const RoomData& data);
    void clearShapes();

    static constexpr cpCollisionType CollisionType = 'room';
};

