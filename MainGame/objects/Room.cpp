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

Room::Room(GameScene& scene) : gameScene(scene)
{
    sf::FloatRect drawingFrame{(ScreenWidth-PlayfieldWidth)/2, (ScreenHeight-PlayfieldHeight)/2,
        PlayfieldWidth, PlayfieldHeight};
    mainLayerTilemap.setDrawingFrame(drawingFrame);
}

Room::~Room()
{
}

void Room::loadRoom(std::string resourceName)
{
    currentRoom = gameScene.getResourceManager().load<RoomData>(resourceName);
    mainLayerTilemap.setTexture(*gameScene.getResourceManager().load<sf::Texture>(currentRoom->textureName));
    mainLayerTilemap.setTileData(currentRoom->mainLayer);

    generateRoomShapes();

    for (const auto& descriptor : currentRoom->gameObjectDescriptors)
    {
        auto obj = createObjectFromDescriptor(gameScene, descriptor);
        if (obj) gameScene.addObject(std::move(obj));
    }
}

void Room::update(std::chrono::steady_clock::time_point curTime) {}

void Room::render(Renderer& renderer)
{
    renderer.pushDrawable(mainLayerTilemap, {}, 1);
}

void Room::generateRoomShapes()
{
    using namespace Chipmunk;

    clearShapes();
    
    auto staticBody = gameScene.getGameSpace().getStaticBody();
    const auto& layer = currentRoom->mainLayer;

    enum NodeType { None, Knee, Ankle };

    struct HorSegment { size_t y, x1, x2; NodeType p1, p2; bool isDown; };
    struct VertSegment { size_t x, y1, y2; NodeType p1, p2; bool isRight; };

    std::vector<HorSegment> horizontalSegments;
    std::vector<VertSegment> verticalSegments;

    bool inSegment = false;
    HorSegment curHSegment;
    for (size_t j = 0; j < layer.height(); j++)
    {
        for (size_t i = 0; i < layer.width(); i++)
        {
            auto tile = layer(i, j);

            if (!inSegment)
            {
                inSegment = true;
                curHSegment.y = j;
                curHSegment.x1 = i;

                switch (tile)
                {
                    case 0: curHSegment.p1 = Knee; curHSegment.isDown = false; break;
                    case 10: curHSegment.p1 = Knee; curHSegment.isDown = true; break;
                    case 4: curHSegment.p1 = Ankle; curHSegment.isDown = false; break;
                    case 9: curHSegment.p1 = Ankle; curHSegment.isDown = true; break;
                    default: inSegment = false;
                }

                if (i == 0)
                {
                    inSegment = true;
                    if (tile == 1) { curHSegment.p1 = None; curHSegment.isDown = false; }
                    else if (tile == 11) { curHSegment.p1 = None; curHSegment.isDown = true; }
                    else inSegment = false;
                }
            }
            else
            {
                inSegment = false;
                curHSegment.x2 = i;

                switch (tile)
                {
                    case 2: case 12: curHSegment.p2 = Knee; break;
                    case 3: case 8: curHSegment.p2 = Ankle; break;
                    default: inSegment = true;
                }

                if (i == layer.width()-1 && (tile == 1 || tile == 11))
                {
                    curHSegment.p2 = None;
                    inSegment = false;
                }

                if (!inSegment) horizontalSegments.push_back(curHSegment);
            }
        }
    }

    inSegment = false;
    VertSegment curVSegment;
    for (size_t i = 0; i < layer.width(); i++)
    {
        for (size_t j = 0; j < layer.height(); j++)
        {
            auto tile = layer(i, j);

            if (!inSegment)
            {
                inSegment = true;
                curVSegment.x = i;
                curVSegment.y1 = j;

                switch (tile)
                {
                    case 0: curVSegment.p1 = Knee; curVSegment.isRight = false; break;
                    case 2: curVSegment.p1 = Knee; curVSegment.isRight = true; break;
                    case 8: curVSegment.p1 = Ankle; curVSegment.isRight = false; break;
                    case 9: curVSegment.p1 = Ankle; curVSegment.isRight = true; break;
                    default: inSegment = false;
                }

                if (i == 0)
                {
                    inSegment = true;
                    if (tile == 5) { curVSegment.p1 = None; curVSegment.isRight = false; }
                    else if (tile == 7) { curVSegment.p1 = None; curVSegment.isRight = true; }
                    else inSegment = false;
                }
            }
            else
            {
                inSegment = false;
                curVSegment.y2 = j;

                switch (tile)
                {
                    case 10: case 12: curVSegment.p2 = Knee; break;
                    case 3: case 4: curVSegment.p2 = Ankle; break;
                    default: inSegment = true;
                }

                if (i == layer.width()-1 && (tile == 5 || tile == 7))
                {
                    curVSegment.p2 = None;
                    inSegment = false;
                }

                if (!inSegment) verticalSegments.push_back(curVSegment);
            }
        }
    }

    for (const auto& seg : horizontalSegments)
    {
        cpVect endPoint1, endPoint2;

        endPoint1.y = endPoint2.y = seg.y * DefaultTileSize + (seg.isDown ? DefaultTileSize - 2 : 2);

        switch(seg.p1)
        {
            case None: endPoint1.x = (seg.x1 - 1) * DefaultTileSize; break;
            case Knee: endPoint1.x = seg.x1 * DefaultTileSize + 10; break;
            case Ankle: endPoint1.x = (seg.x1 + 1) * DefaultTileSize - 2; break;
        }

        switch(seg.p2)
        {
            case None: endPoint2.x = (seg.x2 + 2) * DefaultTileSize; break;
            case Knee: endPoint2.x = (seg.x2 + 1) * DefaultTileSize - 10; break;
            case Ankle: endPoint2.x = seg.x2 * DefaultTileSize + 2; break;
        }

        auto shp = std::make_shared<SegmentShape>(staticBody, endPoint1, endPoint2, 2);
        if (seg.isDown)
        {
            shp->setElasticity(0.6);
            shp->setCollisionType(Room::terrainCollisionType);
        }
        else shp->setCollisionType(Room::groundTerrainCollisionType);
        gameScene.getGameSpace().add(shp);
        roomShapes.push_back(shp);

        if (seg.p1 == Knee)
        {
            cpVect vec = { seg.x1 * (cpFloat)DefaultTileSize + 10.0,
                           seg.y * (cpFloat)DefaultTileSize + (seg.isDown ? DefaultTileSize - 10.0 : 10.0) };
            auto shp = std::make_shared<CircleShape>(staticBody, 9.8, vec);
            if (seg.isDown)
            {
                shp->setElasticity(0.6);
                shp->setCollisionType(Room::terrainCollisionType);
            }
            else shp->setCollisionType(Room::groundTerrainCollisionType);
            gameScene.getGameSpace().add(shp);
            roomShapes.push_back(shp);
        }

        if (seg.p2 == Knee)
        {
            cpVect vec = { (seg.x2 + 1) * (cpFloat)DefaultTileSize - 10.0,
                           seg.y * (cpFloat)DefaultTileSize + (seg.isDown ? DefaultTileSize - 10.0 : 10.0) };
            auto shp = std::make_shared<CircleShape>(staticBody, 9.8, vec);
            if (seg.isDown)
            {
                shp->setElasticity(0.6);
                shp->setCollisionType(Room::terrainCollisionType);
            }
            else shp->setCollisionType(Room::groundTerrainCollisionType);
            gameScene.getGameSpace().add(shp);
            roomShapes.push_back(shp);
        }
    }

    for (const auto& seg : verticalSegments)
    {
        cpVect endPoint1, endPoint2;

        endPoint1.x = endPoint2.x = seg.x * DefaultTileSize + (seg.isRight ? DefaultTileSize - 2 : 2);

        switch(seg.p1)
        {
            case None: endPoint1.y = (seg.y1 - 1) * DefaultTileSize; break;
            case Knee: endPoint1.y = seg.y1 * DefaultTileSize + 10; break;
            case Ankle: endPoint1.y = (seg.y1 + 1) * DefaultTileSize - 2; break;
        }

        switch(seg.p2)
        {
            case None: endPoint2.y = (seg.y2 + 2) * DefaultTileSize; break;
            case Knee: endPoint2.y = (seg.y2 + 1) * DefaultTileSize - 10; break;
            case Ankle: endPoint2.y = seg.y2 * DefaultTileSize + 2; break;
        }

        auto shp = std::make_shared<SegmentShape>(staticBody, endPoint1, endPoint2, 2);
        shp->setElasticity(0.6);
        shp->setCollisionType(Room::wallTerrainCollisionType);
        gameScene.getGameSpace().add(shp);
        roomShapes.push_back(shp);
    }
}

void Room::clearShapes()
{
    for (auto shp : roomShapes)
        gameScene.getGameSpace().remove(shp);

    roomShapes.clear();
}
