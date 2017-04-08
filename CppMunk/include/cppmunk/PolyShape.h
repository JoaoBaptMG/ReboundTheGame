#ifndef CHIPMUNK_POLYSHAPE_H
#define CHIPMUNK_POLYSHAPE_H

#include "Shape.h"
#include <vector>

namespace Chipmunk
{
    class PolyShape : public Shape
    {
    public:
        PolyShape(std::shared_ptr<Body>, const std::vector<cpVect>& verts, cpFloat radius = 0);
        
        /// Get the number of verts in a polygon shape.
        int getCount() const { return cpPolyShapeGetCount(_shape); };
        /// Get the @c ith vertex of a polygon shape.
        cpVect getVert(int i) { return cpPolyShapeGetVert(_shape, i); };
        /// Get the radius of a polygon shape.
        cpFloat getRadius() { return cpPolyShapeGetRadius(_shape); };
    };
}

#endif /* CHIPMUNK_POLYSHAPE_H */
