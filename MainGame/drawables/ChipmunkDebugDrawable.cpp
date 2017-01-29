#include "ChipmunkDebugDrawable.hpp"
#include <chipmunk_structs.h>

#include <algorithm>

sf::Vector2f toVec(cpVect v)
{
    return sf::Vector2f((float)v.x, (float)v.y);
}

auto pointsForShape(cpShape* shape)
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

            size_t partitions = floor(4.2*radius);
            for (int i = 0; i < partitions; i++)
            {
                auto vec = vpos + cpvforangle(6.28318530718 * i / partitions) * radius;
                vertices.emplace_back(toVec(vec), sf::Color::Green);
            }
        } break;

        case CP_SEGMENT_SHAPE:
        {
            auto radius = cpSegmentShapeGetRadius(shape);
            auto a = pos + cpvrotate(rvec, cpSegmentShapeGetA(shape));
            auto b = pos + cpvrotate(rvec, cpSegmentShapeGetB(shape));
            auto prp = cpvnormalize(cpvrperp(b-a)) * radius;

            size_t partitions = floor(2.1*radius);

            for (int i = 0; i < partitions; i++)
            {
                auto avec = cpvforangle(6.28318530718/2 * i / partitions);
                auto vec = a + cpvrotate(prp, avec);
                vertices.emplace_back(toVec(vec), sf::Color::Green);
            }
            vertices.emplace_back(toVec(a - prp), sf::Color::Green);

            for (int i = 0; i < partitions; i++)
            {
                auto avec = cpvforangle(6.28318530718/2 * i / partitions);
                auto vec = b - cpvunrotate(prp, avec);
                vertices.emplace_back(toVec(vec), sf::Color::Green);
            }
            vertices.emplace_back(toVec(b + prp), sf::Color::Green);
        } break;

        default: break;
    }

    return vertices;
}

void ChipmunkDebugDrawable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    struct RenderData { sf::RenderTarget& target; sf::RenderStates states; }
    renderData{target, states};

    // hack
    cpSpace* space = const_cast<Chipmunk::Space&>(debugSpace);
    cpSpaceEachShape(space, [](cpShape* shape, void* data)
    {
        RenderData& renderData = *(RenderData*)data;
        
        auto pts = pointsForShape(shape);
        pts.push_back(pts[0]);

        renderData.target.draw(pts.data(), pts.size(), sf::PrimitiveType::TriangleFan, renderData.states);
    }, &renderData);
}
