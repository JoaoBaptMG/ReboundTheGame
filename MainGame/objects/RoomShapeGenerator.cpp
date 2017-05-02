#include <vector>
#include <memory>

#include <cppmunk/Space.h>
#include <cppmunk/Body.h>
#include <cppmunk/SegmentShape.h>
#include <cppmunk/CircleShape.h>
#include "data/RoomData.hpp"
#include "data/TileSet.hpp"
#include "defaults.hpp"

std::vector<std::shared_ptr<cp::Shape>>
    generateShapesForTilemap(const RoomData& data, const TileSet& tileSet, std::shared_ptr<cp::Body> body)
{
    const auto& layer = data.mainLayer;

    enum NodeType { None, Knee, Ankle };
    struct HorSegment { ssize_t y, x1, x2; };
    struct VertSegment { ssize_t x, y1, y2; };

    auto isSolid = [&](ssize_t x, ssize_t y)
    {
        return tileSet.tileModes[std::min<uint8_t>(layer(x, y),
            tileSet.tileModes.size()-1)] == TileSet::Mode::Solid;
    };
    auto horizontalFootprint = [&](ssize_t j, ssize_t i) -> ssize_t
    {
        return (ssize_t(isSolid(i, j-1)) << 1) | ssize_t(isSolid(i, j));
    };
    auto verticalFootprint = [&](ssize_t j, ssize_t i) -> ssize_t
    {
        return (ssize_t(isSolid(j-1, i)) << 1) | ssize_t(isSolid(j, i));
    };

    std::vector<HorSegment> horizontalSegments;
    std::vector<VertSegment> verticalSegments;

/*#define GEN_SEGMENTS(perp, par, dir)                                                 \
    do                                                                               \
    {                                                                                \
        for (ssize_t j = 1; j < layer.perp(); j++)                                   \
        {                                                                            \
            ssize_t i = 0;                                                           \
                                                                                     \
            while (i < layer.par())                                                  \
            {                                                                        \
                auto previ = i;                                                      \
                auto curFootprint = dir##Footprint(j, i);                            \
                while (i < layer.par() && curFootprint == dir##Footprint(j, i)) i++; \
                                                                                     \
                if (curFootprint == 1 || curFootprint == 2)                          \
                    dir##Segments.push_back({ j, previ, i });                        \
            }                                                                        \
        }                                                                            \
    } while(0)

    GEN_SEGMENTS(height, width, horizontal);
    GEN_SEGMENTS(width, height, vertical);
#undef GEN_SEGMENTS*/

    do
    {
        for (ssize_t j = 1; j < layer.height(); j++)
        {
            ssize_t i = 0;
            while (i < layer.width())
            {
                auto previ = i;
                auto curFootprint = horizontalFootprint(j, i);
                while (i < layer.width() && curFootprint == horizontalFootprint(j, i)) i++;
                if (curFootprint == 1 || curFootprint == 2)
                    horizontalSegments.push_back({ j, previ, i });
            }
        }
    } while(0);
    
    do
    {
        for (ssize_t j = 1; j < layer.width(); j++)
        {
            ssize_t i = 0;
            while (i < layer.height())
            {
                auto previ = i;
                auto curFootprint = verticalFootprint(j, i);
                while (i < layer.height() && curFootprint == verticalFootprint(j, i)) i++;
                if (curFootprint == 1 || curFootprint == 2)
                    verticalSegments.push_back({ j, previ, i });
            }
        }
    } while(0);

    std::vector<std::shared_ptr<cp::Shape>> shapes;
    const auto& params = tileSet.physicalParameters;

#define CONVERT_SHAPES(j, i, perpnOffset, perppOffset, parnOffset, parpOffset, segments, solid)       \
    do                                                                                                \
    {                                                                                                 \
        for (auto seg : segments)                                                                     \
        {                                                                                             \
            cpVect endPoint1, endPoint2;                                                              \
                                                                                                      \
            bool isDown = solid(seg.i##1, seg.j);                                                     \
            endPoint1.j = endPoint2.j = DefaultTileSize * seg.j +                                     \
                (isDown ? -params.perpnOffset + params.cornerRadius                                   \
                        :  params.perppOffset - params.cornerRadius);                                 \
                                                                                                      \
            if (seg.i##1 == 0) endPoint1.i = -(ssize_t)DefaultTileSize;                               \
            else                                                                                      \
            {                                                                                         \
                endPoint1.i = DefaultTileSize * seg.i##1;                                             \
                if (isDown ? solid(seg.i##1-1, seg.j) : solid(seg.i##1-1, seg.j-1))                   \
                    endPoint1.i += params.parpOffset - params.cornerRadius;                           \
                else endPoint1.i -= params.parnOffset - params.cornerRadius;                          \
            }                                                                                         \
                                                                                                      \
            if (seg.i##2 == bounds) endPoint2.i = (bounds + 1) * DefaultTileSize;                     \
            else                                                                                      \
            {                                                                                         \
                endPoint2.i = DefaultTileSize * seg.i##2;                                             \
                if (isDown ? solid(seg.i##2, seg.j) : solid(seg.i##2, seg.j-1))                       \
                    endPoint2.i -= params.parnOffset - params.cornerRadius;                           \
                else endPoint2.i += params.parpOffset - params.cornerRadius;                          \
            }                                                                                         \
                                                                                                      \
            shapes.push_back(std::make_shared<cp::SegmentShape>                                       \
                (body, endPoint1, endPoint2, params.cornerRadius));                                   \
        }                                                                                             \
    } while (0)

    size_t bounds = layer.width();
    CONVERT_SHAPES(y, x, upperOffset, lowerOffset, leftOffset, rightOffset, horizontalSegments, isSolid);

    bounds = layer.height();
    auto isReverseSolid = [&](ssize_t i, ssize_t j) { return isSolid(j, i); };
    CONVERT_SHAPES(x, y, leftOffset, rightOffset, upperOffset, lowerOffset, verticalSegments, isReverseSolid);

    return shapes;
#undef CONVERT_SHAPES
}
