#include "ChipmunkDebugDrawable.hpp"
#include <chipmunk/chipmunk_structs.h>

#include <algorithm>

sf::Vector2f toVec(cpVect v)
{
    return sf::Vector2f((float)v.x, (float)v.y);
}

auto pointsForShape(cpShape* shape, sf::Color color)
{
    std::vector<sf::Vertex> vertices;

    auto pos = cpBodyGetPosition(cpShapeGetBody(shape));
    auto rvec = cpBodyGetRotation(cpShapeGetBody(shape));

    switch (shape->klass->type)
    {
        case CP_CIRCLE_SHAPE:
        {
            auto radius = cpCircleShapeGetRadius(shape);
            auto vpos = pos + cpvrotate(rvec, cpCircleShapeGetOffset(shape));

            uintmax_t partitions = floor(4.2*radius);
            for (int i = 0; i < partitions; i++)
            {
                auto vec = vpos + cpvforangle(6.28318530718 * i / partitions) * radius;
                vertices.emplace_back(toVec(vec), color);
            }
        } break;

        case CP_SEGMENT_SHAPE:
        {
            auto radius = cpSegmentShapeGetRadius(shape);
            auto a = pos + cpvrotate(rvec, cpSegmentShapeGetA(shape));
            auto b = pos + cpvrotate(rvec, cpSegmentShapeGetB(shape));
            auto prp = cpvnormalize(cpvrperp(b-a)) * radius;

            uintmax_t partitions = floor(2.1*radius);

            for (int i = 0; i < partitions; i++)
            {
                auto avec = cpvforangle(6.28318530718/2 * i / partitions);
                auto vec = a - cpvrotate(prp, avec);
                vertices.emplace_back(toVec(vec), color);
            }
            vertices.emplace_back(toVec(a + prp), color);

            for (int i = 0; i < partitions; i++)
            {
                auto bvec = cpvforangle(6.28318530718/2 * i / partitions);
                auto vec = b + cpvrotate(prp, bvec);
                vertices.emplace_back(toVec(vec), color);
            }
            vertices.emplace_back(toVec(b - prp), color);
        } break;

        case CP_POLY_SHAPE:
        {
            auto n = cpPolyShapeGetCount(shape);
            auto radius = cpPolyShapeGetRadius(shape);

            for (int i = 0; i < n; i++)
            {
                if (radius == 0)
                {
                    auto vpos = pos + cpvrotate(rvec, cpPolyShapeGetVert(shape, i));
                    vertices.emplace_back(toVec(vpos), color);
                }
                else
                {
                    auto vprev = pos + cpvrotate(rvec, cpPolyShapeGetVert(shape, i != 0 ? i-1 : n-1));
                    auto vcur = pos + cpvrotate(rvec, cpPolyShapeGetVert(shape, i));
                    auto vnext = pos + cpvrotate(rvec, cpPolyShapeGetVert(shape, i != n-1 ? i+1 : 0));

                    auto t1 = cpvtoangle(cpvrperp(vcur - vprev));
                    auto t2 = cpvtoangle(cpvperp(vcur - vnext));

                    t1 -= floor(t1/6.28318530718) * 6.28318530718;
                    t2 -= floor(t2/6.28318530718) * 6.28318530718;
                    if (t2 < t1) t2 += 6.28318530718;

                    uintmax_t partitions = floor((t2-t1)*radius / 3);

                    for (int i = 0; i < partitions; i++)
                    {
                        auto vec = vcur + cpvforangle(t1 + (t2-t1) * i / partitions) * radius;
                        vertices.emplace_back(toVec(vec), color);
                    }

                    vertices.emplace_back(toVec(vcur + cpvforangle(t2) * radius), color);
                }
            }
        } break;

        default: break;
    }

    return vertices;
}

void ChipmunkDebugDrawable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.blendMode = sf::BlendAlpha;
    struct RenderData { sf::RenderTarget& target; sf::RenderStates states; }
    renderData{target, states};

    // hack
    cpSpace* space = const_cast<cp::Space&>(debugSpace);
    cpSpaceEachShape(space, [](cpShape* shape, void* data)
    {
        RenderData& renderData = *(RenderData*)data;
        
        auto pts = pointsForShape(shape, sf::Color(0, 255, 0, 128));
        pts.push_back(pts[0]);

        renderData.target.draw(pts.data(), pts.size(), sf::PrimitiveType::TriangleFan, renderData.states);
    }, &renderData);
}
