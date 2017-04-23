#ifndef CHIPMUNK_CIRCLESHAPE_H
#define CHIPMUNK_CIRCLESHAPE_H

#include "Shape.h"

namespace cp
{
    class CircleShape : public Shape
    {
    public:
        CircleShape(std::shared_ptr<Body>, cpFloat radius, cpVect offset = cpv(0, 0));
        
        /// Get the offset of a circle shape.
        inline cpVect getOffset() const { return cpCircleShapeGetOffset(_shape); };
        /// Get the radius of a circle shape.
        inline cpFloat getRadius() const { return cpCircleShapeGetRadius(_shape); };
    };
}

#endif /* CHIPMUNK_CIRCLESHAPE_H */
