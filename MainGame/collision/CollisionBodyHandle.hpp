#pragma once

#include <vector>
#include "chipmunkWrapperTypes.hpp"

class CollisionHandler;

namespace cp
{
    class CollisionBodyHandle final
    {
        cp::BodyPtr body;
        std::vector<cp::ShapePtr> shapes;
        cpSpace* space;

        CollisionBodyHandle(cpSpace *space, cp::BodyPtr &&body) : space(space), body(std::move(body))
        {
            cpSpaceAddBody(space, this->body.get());
        }

    public:
        CollisionBodyHandle() {}

        cpBody* getBodyPtr() const { return body.get(); }
        cpShape* getShapePtr(size_t id) const { return shapes.at(id).get(); }

        void clearShapes()
        {
            for (const auto& shape : shapes) cpSpaceRemoveShape(space, shape.get());
            shapes.clear();
        }

        cpShape* addShape(cp::ShapePtr &&shape)
        {
            cpSpaceAddShape(space, shape.get());
            shapes.push_back(std::move(shape));
            return shapes.back().get();
        }

        void setShapes(std::vector<cp::ShapePtr> &&shapes)
        {
            clearShapes();
            for (auto&& shape : shapes) addShape(std::move(shape));
        }

        friend class ::CollisionHandler;
    };
}
