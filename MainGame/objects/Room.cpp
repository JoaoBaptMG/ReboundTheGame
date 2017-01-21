#include "Room.hpp"
#include <functional>
#include "../scene/GameScene.hpp"
#include "../collision/CollisionHandler.hpp"
#include "../defaults.hpp"

Room::Room(GameScene& scene, ResourceManager& manager) : currentScene(scene), currentManager(manager)
{
    handle = scene.getCollisionHandler().newStaticBody();

    sf::FloatRect drawingFrame{(ScreenWidth-PlayfieldWidth)/2, (ScreenHeight-PlayfieldHeight)/2,
        PlayfieldWidth, PlayfieldHeight};
    mainLayerTilemap.setDrawingFrame(drawingFrame);
}

Room::~Room()
{
}

void Room::loadRoom(std::string resourceName)
{
    currentRoom = currentManager.load<RoomData>(resourceName);
    mainLayerTilemap.setTexture(*currentManager.load<sf::Texture>(currentRoom->textureName));
    mainLayerTilemap.setTileData(currentRoom->mainLayer);

    generateRoomShapes();
}

void Room::update(float delta)
{

}

void Room::render(Renderer& renderer)
{
    renderer.pushDrawable(mainLayerTilemap, {}, 1);
}

void Room::generateRoomShapes()
{
    handle.clearShapes();

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
            case None: endPoint2.x = (seg.x2 + 1) * DefaultTileSize; break;
            case Knee: endPoint2.x = (seg.x2 + 1) * DefaultTileSize - 10; break;
            case Ankle: endPoint2.x = seg.x2 * DefaultTileSize + 2; break;
        }

        auto shp = handle.addShape(cp::SegmentShapeMake(handle.getBodyPtr(), endPoint1, endPoint2, 2));
        cpShapeSetFriction(shp, 0.9);

        if (seg.p1 == Knee)
        {
            cpVect vec = { seg.x1 * (cpFloat)DefaultTileSize + 10.0, seg.y * (cpFloat)DefaultTileSize + 10.0 };
            auto shp = handle.addShape(cp::CircleShapeMake(handle.getBodyPtr(), 10, vec));
            cpShapeSetFriction(shp, 0.9);
        }

        if (seg.p2 == Knee)
        {
            cpVect vec = { (seg.x2 + 1) * (cpFloat)DefaultTileSize - 10.0, seg.y * (cpFloat)DefaultTileSize + 10.0 };
            auto shp = handle.addShape(cp::CircleShapeMake(handle.getBodyPtr(), 10, vec));
            cpShapeSetFriction(shp, 0.9);
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
            case None: endPoint2.y = (seg.y2 + 1) * DefaultTileSize; break;
            case Knee: endPoint2.y = seg.y2 * DefaultTileSize - 10; break;
            case Ankle: endPoint2.y = (seg.y2 - 1) * DefaultTileSize + 2; break;
        }

        auto shp = handle.addShape(cp::SegmentShapeMake(handle.getBodyPtr(), endPoint1, endPoint2, 2));
        cpShapeSetFriction(shp, 1.0);
        cpShapeSetElasticity(shp, 1.0);
    }
}
