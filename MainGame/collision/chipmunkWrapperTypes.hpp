#pragma once

#include <chipmunk.h>
#include <memory>
#include <utility>
#include <SFML/Graphics.hpp>
#include "../utility/non_copyable_movable.hpp"

namespace cp
{
    inline sf::Vector2<cpFloat> convert(cpVect v) { return sf::Vector2<cpFloat>(v.x, v.y); }
    inline sf::Rect<cpFloat> convert(cpBB b) { return sf::Rect<cpFloat>(b.l, b.b, b.r - b.r, b.t - b.b); }

    template <typename T, void(*func)(T*)>
    struct CpFunctionDeleter { void operator()(T* val) { func(val); } };

    template <typename T, void(*func)(T*)>
    using CpUniquePtr = std::unique_ptr<T, CpFunctionDeleter<T, func>>;

    using BodyPtr = CpUniquePtr<cpBody, cpBodyFree>;
    using ShapePtr = CpUniquePtr<cpShape, cpShapeFree>;
    using SpacePtr = CpUniquePtr<cpSpace, cpSpaceFree>;

    inline BodyPtr BodyMake(cpFloat m, cpFloat i) { return BodyPtr(cpBodyNew(m, i)); }
    inline BodyPtr BodyMakeKinematic() { return BodyPtr(cpBodyNewKinematic()); }
    inline BodyPtr BodyMakeStatic() { return BodyPtr(cpBodyNewStatic()); }

    inline ShapePtr CircleShapeMake(cpBody* body, cpFloat radius, cpVect offset)
    {
        return ShapePtr(cpCircleShapeNew(body, radius, offset));
    }

    inline ShapePtr SegmentShapeMake(cpBody* body, cpVect a, cpVect b, cpFloat radius)
    {
        return ShapePtr(cpSegmentShapeNew(body, a, b, radius));
    }

    inline ShapePtr PolyShapeMake(cpBody* body, int n, cpVect* verts, cpTransform transform, cpFloat radius)
    {
        return ShapePtr(cpPolyShapeNew(body, n, verts, transform, radius));
    }

    template <size_t N>
    inline ShapePtr PolyShapeMake(cpBody* body, cpVect(&verts)[N], cpTransform transform, cpFloat radius)
    {
        return PolyShapeMake(body, N, verts, transform, radius);
    }

    inline ShapePtr PolyShapeMake(cpBody* body, std::vector<cpVect> verts, cpTransform transform, cpFloat radius)
    {
        return PolyShapeMake(body, verts.size(), verts.data(), transform, radius);
    }

    inline ShapePtr BoxShapeMake(cpBody* body, cpFloat width, cpFloat height, cpFloat radius)
    {
        return ShapePtr(cpBoxShapeNew(body, width, height, radius));
    }

    inline ShapePtr BoxShapeMake(cpBody* body, cpBB bb, cpFloat radius)
    {
        return ShapePtr(cpBoxShapeNew2(body, bb, radius));
    }

    inline SpacePtr SpaceMake()
    {
        return SpacePtr(cpSpaceNew());
    }

    inline void BodyEachShape(cpBody *body, std::function<void(cpBody*,cpShape*)> callback)
    {
        cpBodyEachShape(body, [](cpBody* body, cpShape* shape, void* data)
        {
            (*(decltype(callback)*)data)(body, shape);
        }, (void*)&callback);
    }

    inline void BodyEachConstraint(cpBody *body, std::function<void(cpBody*,cpConstraint*)> callback)
    {
        cpBodyEachConstraint(body, [](cpBody* body, cpConstraint* constraint, void* data)
        {
            (*(decltype(callback)*)data)(body, constraint);
        }, (void*)&callback);
    }

    inline void BodyEachArbiter(cpBody *body, std::function<void(cpBody*,cpArbiter*)> callback)
    {
        cpBodyEachArbiter(body, [](cpBody* body, cpArbiter* arbiter, void* data)
        {
            (*(decltype(callback)*)data)(body, arbiter);
        }, (void*)&callback);
    }
}
