#ifndef CHIPMUNK_SEGMENTSHAPE_H
#define CHIPMUNK_SEGMENTSHAPE_H

#include "Shape.h"
#include "Body.h"
#include <memory>

namespace cp
{
    class SegmentShape : public Shape
    {
    public:
        SegmentShape(std::shared_ptr<Body>,
                     cpVect a,
                     cpVect b,
                     cpFloat radius);
        
        /// Let Chipmunk know about the geometry of adjacent segments to avoid colliding with endcaps.
        void setNeighbors(std::shared_ptr<Shape> shape, cpVect prev, cpVect next);
        
        /// Get the first endpoint of a segment shape.
        cpVect getA();
        /// Get the second endpoint of a segment shape.
        cpVect getB();
        /// Get the normal of a segment shape.
        cpVect getNormal();
        /// Get the first endpoint of a segment shape.
        cpFloat getRadius();
    };
}

#endif /* CHIPMUNK_SEGMENTSHAPE_H */
